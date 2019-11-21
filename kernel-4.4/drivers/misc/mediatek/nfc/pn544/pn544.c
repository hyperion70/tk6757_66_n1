/*
* Copyright (C) 2016 MediaTek Inc.
 *
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 as
* published by the Free Software Foundation.
 *
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
* See http://www.gnu.org/licenses/gpl-2.0.html for more details.
*/

#define pr_fmt(fmt) "["KBUILD_MODNAME"]" fmt
/*****************************************************************************
 * Include
 *****************************************************************************/
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/i2c.h>
#include <linux/irq.h>
#include <linux/jiffies.h>
#include <linux/uaccess.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/miscdevice.h>
#include <linux/spinlock.h>
#include <linux/dma-mapping.h>
#include <linux/interrupt.h>

#ifdef CONFIG_OF
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>
#endif

/* #include <mach/eint.h> */
/* #include <mach/pn544.h> */

#ifndef CONFIG_MTK_FPGA
#include <mtk_clkbuf_ctl.h>
#endif

/* #include <cust_eint.h> */
/* #include <cust_i2c.h>  */

#if defined(CONFIG_MTK_LEGACY)
#include <mach/mt_gpio.h>
#include <cust_gpio_usage.h>
#endif

/*****************************************************************************
 * Define
 *****************************************************************************/

/* #define NFC_I2C_BUSNUM  I2C_NFC_CHANNEL */

#define I2C_ID_NAME "pn544"

#define MAX_BUFFER_SIZE	512

#define NFC_CLIENT_TIMING 400	/* I2C speed */

enum {
	MTK_NFC_GPIO_EN_B = 0x0,
	MTK_NFC_GPIO_SYSRST_B,
	MTK_NFC_GPIO_EINT,
	MTK_NFC_GPIO_IRQ,
	MTK_NFC_GPIO_IOCTL,
	MTK_NFC_GPIO_MAX_NUM
};

enum {
	MTK_NFC_IOCTL_READ = 0x0,
	MTK_NFC_IOCTL_WRITE,
	MTK_NFC_IOCTL_MAX_NUM
};

enum {
	MTK_NFC_IOCTL_CMD_CLOCK_BUF_ENABLE = 0x0,
	MTK_NFC_IOCTL_CMD_CLOCK_BUF_DISABLE,
	MTK_NFC_IOCTL_CMD_EXIT_EINT,
	MTK_NFC_IOCTL_CMD_GET_CHIP_ID,
	MTK_NFC_IOCTL_CMD_READ_DATA,
	MTK_NFC_IOCTL_CMD_MAX_NUM
};

enum {
	MTK_NFC_PULL_LOW = 0x0,
	MTK_NFC_PULL_HIGH,
	MTK_NFC_PULL_INVALID,
};

enum {
	MTK_NFC_GPIO_DIR_IN = 0x0,
	MTK_NFC_GPIO_DIR_OUT,
	MTK_NFC_GPIO_DIR_INVALID,
};

/*****************************************************************************
 * Global Variable
 *****************************************************************************/
struct pn544_dev *pn544_dev_ptr;
struct pn544_dev _gpn544_dev;

#if !defined(CONFIG_MTK_LEGACY)

struct pinctrl *gpctrl;
struct pinctrl_state *st_ven_h;
struct pinctrl_state *st_ven_l;
struct pinctrl_state *st_rst_h;
struct pinctrl_state *st_rst_l;
struct pinctrl_state *st_eint_h;
struct pinctrl_state *st_eint_l;
struct pinctrl_state *st_irq_init;
struct pinctrl_state *st_osc_init;
#endif

/* static struct i2c_board_info nfc_board_info __initdata = */
/*    { I2C_BOARD_INFO(I2C_ID_NAME, I2C_NFC_SLAVE_7_BIT_ADDR) }; */

/* For DMA */
#ifdef CONFIG_MTK_I2C_EXTENSION
static char *I2CDMAWriteBuf;
static unsigned int I2CDMAWriteBuf_pa;
static char *I2CDMAReadBuf;
static unsigned int I2CDMAReadBuf_pa;
#else
static char I2CDMAWriteBuf[MAX_BUFFER_SIZE];
static char I2CDMAReadBuf[MAX_BUFFER_SIZE];
#endif
static int fgNfcChip;
int forceExitBlockingRead;

/*  NFC IRQ */
static u32 nfc_irq;
static int nfc_irq_count;	/*= 0;*//* unnecessary initialise */

/*****************************************************************************
 * Function Prototype
 *****************************************************************************/
static int pn544_probe(struct i2c_client *client,
			const struct i2c_device_id *id);
static int pn544_remove(struct i2c_client *client);
/* static int pn544_detect(struct i2c_client *client, */
/*                                int kind, struct i2c_board_info *info); */
static int pn544_dev_open(struct inode *inode, struct file *filp);
static long pn544_dev_unlocked_ioctl(struct file *filp, unsigned int cmd,
				      unsigned long arg);
static ssize_t pn544_dev_read(struct file *filp, char __user *buf,
			       size_t count, loff_t *offset);
static ssize_t pn544_dev_write(struct file *filp, const char __user *buf,
				size_t count, loff_t *offset);

static int mt_nfc_probe(struct platform_device *pdev);
static int mt_nfc_remove(struct platform_device *pdev);

/* void pn544_dev_irq_handler(void); */
static irqreturn_t pn544_dev_irq_handler(int irq, void *data);	/*IRQ handler */

/* static void pn544_disable_irq(struct pn544_dev *pn544_dev); */
#if !defined(CONFIG_MTK_LEGACY)
static int mt_nfc_pinctrl_init(struct platform_device *pdev);
static int mt_nfc_pinctrl_select(struct pinctrl *p, struct pinctrl_state *s);
static int mt_nfc_gpio_init(void);
#endif
static int mt_nfc_get_gpio_value(int gpio_num);
static int mt_nfc_get_gpio_dir(int gpio_num);

/*****************************************************************************
 * Data Structure
 *****************************************************************************/

struct pn544_dev {
	wait_queue_head_t read_wq;
	struct mutex read_mutex;
	struct i2c_client *client;
	struct miscdevice pn544_device;
	unsigned int ven_gpio;
	unsigned int sysrstb_gpio;
	unsigned int irq_gpio;	/* Chip inform Host */
	unsigned int eint_gpio;	/* Host inform Chip */
	bool irq_enabled;
	struct mutex irq_enabled_lock;
	/* spinlock_t            irq_enabled_lock; */
};

struct pn544_i2c_platform_data {
	unsigned int irq_gpio;	/* Chip inform Host */
	unsigned int ven_gpio;
	unsigned int sysrstb_gpio;
	unsigned int eint_gpio;	/* Host inform Chip */
	unsigned int osc_en;
};

static const struct i2c_device_id pn544_id[] = {
	{I2C_ID_NAME, 0},
	{}
};

#ifdef CONFIG_OF
static const struct of_device_id nfc_switch_of_match[] = {
	{.compatible = "mediatek,nfc"},
	{},
};
#endif

/* #if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)) */
/* static struct i2c_client_address_data addr_data = { .forces = forces,}; */
/* #endif */
static struct i2c_driver pn544_dev_driver = {
	.id_table = pn544_id,
	.probe = pn544_probe,
	.remove = pn544_remove,
	/* .detect               = pn544_detect, */
	.driver = {
		   .name = "pn544",
		   .owner = THIS_MODULE,
#ifdef CONFIG_OF
		   .of_match_table = nfc_switch_of_match,
#endif
		   },
};

/*  platform driver */
static const struct of_device_id nfc_dev_of_match[] = {
	{.compatible = "mediatek,nfc-gpio-v2",},
	{},
};

static struct platform_driver mtk_nfc_platform_driver = {
	.probe = mt_nfc_probe,
	.remove = mt_nfc_remove,
	.driver = {
		   .name = I2C_ID_NAME,
		   .owner = THIS_MODULE,
#ifdef CONFIG_OF
		   .of_match_table = nfc_dev_of_match,
#endif
		   },
};

#if !defined(CONFIG_MTK_LEGACY)
struct pn544_i2c_platform_data pn544_platform_data;
#else
static struct pn544_i2c_platform_data pn544_platform_data = {
	.irq_gpio = GPIO_IRQ_NFC_PIN,
	.ven_gpio = GPIO_NFC_VENB_PIN,
	.sysrstb_gpio = GPIO_NFC_RST_PIN,
	.eint_gpio = GPIO_NFC_EINT_PIN,
};
#endif

static const struct file_operations pn544_dev_fops = {
	.owner = THIS_MODULE,
	.llseek = no_llseek,
	.read = pn544_dev_read,
	.write = pn544_dev_write,
	.open = pn544_dev_open,
#ifdef CONFIG_COMPAT
	.compat_ioctl = pn544_dev_unlocked_ioctl,
#endif
	.unlocked_ioctl = pn544_dev_unlocked_ioctl,
};

/*****************************************************************************
 * Extern Area
 *****************************************************************************/

/*****************************************************************************
 * Function
 *****************************************************************************/
static void pn544_enable_irq(u32 irq_line)
{
	nfc_irq_count++;	/* It must set before call enable_irq */

	enable_irq(irq_line);

	/* pr_debug("%s : nfc_irq_count = %d.\n", __func__, nfc_irq_count); */
}

static void pn544_disable_irq(u32 irq_line)
{
	if (nfc_irq_count >= 1) {
		disable_irq_nosync(irq_line);
		nfc_irq_count--;
	} else {
		pr_debug("%s : disable irq fail.\n", __func__);
	}
	/* pr_debug("%s : nfc_irq_count = %d.\n", __func__, nfc_irq_count); */
}

static int pn544_probe(struct i2c_client *client,
			const struct i2c_device_id *id)
{
	int ret = 0;
	struct pn544_i2c_platform_data *platform_data;
	struct device_node *node;

	pr_debug("pn544_dev_probe\n");

	platform_data = &pn544_platform_data;

	if (platform_data == NULL) {
		pr_err("%s : nfc probe fail\n", __func__);
		return -ENODEV;
	}

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		pr_err("%s : need I2C_FUNC_I2C\n", __func__);
		return -ENODEV;
	}
#if defined(CONFIG_MTK_LEGACY)
	mt_set_gpio_mode(platform_data->irq_gpio, GPIO_IRQ_NFC_PIN_M_GPIO);
	mt_set_gpio_dir(platform_data->irq_gpio, GPIO_DIR_IN);
	mt_set_gpio_pull_enable(platform_data->irq_gpio, GPIO_PULL_ENABLE);
	mt_set_gpio_pull_select(platform_data->irq_gpio, GPIO_PULL_DOWN);

	/* ven_gpio */
	mt_set_gpio_mode(platform_data->ven_gpio, GPIO_NFC_VENB_PIN_M_GPIO);
	mt_set_gpio_dir(platform_data->ven_gpio, GPIO_DIR_OUT);
	mt_set_gpio_out(platform_data->ven_gpio, GPIO_OUT_ONE);
	usleep_range(900, 1000);	/* debug */

	/* firm_gpio */
	mt_set_gpio_mode(platform_data->sysrstb_gpio, GPIO_NFC_RST_PIN_M_GPIO);
	mt_set_gpio_dir(platform_data->sysrstb_gpio, GPIO_DIR_OUT);
	mt_set_gpio_out(platform_data->sysrstb_gpio, GPIO_OUT_ONE);
	usleep_range(900, 1000);	/* debug */

	/* EINT_gpio */
	mt_set_gpio_mode(platform_data->eint_gpio, GPIO_NFC_EINT_PIN_M_GPIO);
	mt_set_gpio_dir(platform_data->eint_gpio, GPIO_DIR_OUT);
	/* Set output High */
	mt_set_gpio_out(platform_data->eint_gpio, GPIO_OUT_ZERO);

	_gpn544_dev.irq_gpio = platform_data->irq_gpio;
	_gpn544_dev.ven_gpio = platform_data->ven_gpio;
	_gpn544_dev.sysrstb_gpio = platform_data->sysrstb_gpio;
	_gpn544_dev.eint_gpio = platform_data->eint_gpio;
#endif

	_gpn544_dev.client = client;
	/* init mutex and queues */
	init_waitqueue_head(&_gpn544_dev.read_wq);
	mutex_init(&_gpn544_dev.read_mutex);
	/* spin_lock_init(&pn544_dev->irq_enabled_lock); */
	mutex_init(&_gpn544_dev.irq_enabled_lock);

#if 0
	_gpn544_dev.pn544_device.minor = MISC_DYNAMIC_MINOR;
	_gpn544_dev.pn544_device.name = "pn544";
	_gpn544_dev.pn544_device.fops = &pn544_dev_fops;

	ret = misc_register(&_gpn544_dev.pn544_device);
	if (ret) {
		pr_err("%s : misc_register failed\n", __FILE__);
		goto err_misc_register;
	}
#endif

#ifdef CONFIG_MTK_I2C_EXTENSION
#ifdef CONFIG_64BIT
	I2CDMAWriteBuf =
	    (char *)dma_alloc_coherent(&client->dev, MAX_BUFFER_SIZE,
				       (dma_addr_t *) &I2CDMAWriteBuf_pa,
				       GFP_KERNEL);
#else
	I2CDMAWriteBuf =
	    (char *)dma_alloc_coherent(NULL, MAX_BUFFER_SIZE,
				       (dma_addr_t *) &I2CDMAWriteBuf_pa,
				       GFP_KERNEL);
#endif

	if (I2CDMAWriteBuf == NULL) {
		pr_err("%s : failed to allocate dma buffer\n", __func__);
		mutex_destroy(&_gpn544_dev.read_mutex);
		gpio_free(platform_data->sysrstb_gpio);
		return ret;
	}
#ifdef CONFIG_64BIT
	I2CDMAReadBuf =
	    (char *)dma_alloc_coherent(&client->dev, MAX_BUFFER_SIZE,
				       (dma_addr_t *) &I2CDMAReadBuf_pa,
				       GFP_KERNEL);
#else
	I2CDMAReadBuf =
	    (char *)dma_alloc_coherent(NULL, MAX_BUFFER_SIZE,
				       (dma_addr_t *) &I2CDMAReadBuf_pa,
				       GFP_KERNEL);
#endif

	if (I2CDMAReadBuf == NULL) {
		pr_err("%s : failed to allocate dma buffer\n", __func__);
		mutex_destroy(&_gpn544_dev.read_mutex);
		gpio_free(platform_data->sysrstb_gpio);
		return ret;
	}
	pr_debug("%s :I2CDMAWriteBuf_pa %d, I2CDMAReadBuf_pa,%d\n", __func__,
		 I2CDMAWriteBuf_pa, I2CDMAReadBuf_pa);
#else
	memset(I2CDMAWriteBuf, 0x00, sizeof(I2CDMAWriteBuf));
	memset(I2CDMAReadBuf, 0x00, sizeof(I2CDMAReadBuf));
#endif
	/* request irq.  the irq is set whenever the chip has data available
	 * for reading.  it is cleared when all data has been read.
	 */

	/* client->irq = CUST_EINT_IRQ_NFC_NUM; */
	/* pr_debug("%s : requesting IRQ %d\n", __func__, client->irq);  */
	/* pr_debug("pn544_Prob2\n");     */
	/* mt_eint_set_hw_debounce(CUST_EINT_IRQ_NFC_NUM, */
	/*                              CUST_EINT_IRQ_NFC_DEBOUNCE_CN); */
	/* mt_eint_registration(CUST_EINT_IRQ_NFC_NUM,  */
	/*                   CUST_EINT_IRQ_NFC_TYPE, pn544_dev_irq_handler, */
	/*                   0); */

	/*  NFC IRQ settings     */
	node = of_find_compatible_node(NULL, NULL, "mediatek,nfc-gpio-v2");

	if (node) {

		nfc_irq = irq_of_parse_and_map(node, 0);

		client->irq = nfc_irq;

		ret =
		    request_irq(nfc_irq, pn544_dev_irq_handler,
				IRQF_TRIGGER_NONE, "irq_nfc-eint", NULL);

		if (ret) {

			pr_err("%s : EINT IRQ LINE NOT AVAILABLE, ret = %d\n", __func__, ret);

		} else {

			pr_debug("%s : set EINT finished, nfc_irq=%d", __func__,
				 nfc_irq);
			nfc_irq_count++;
			pn544_disable_irq(nfc_irq);
			enable_irq_wake(nfc_irq);
		}

	} else {
		pr_err("%s : can not find NFC eint compatible node\n",
		       __func__);
	}

	/* mt_eint_unmask(CUST_EINT_IRQ_NFC_NUM); */
	/* pn544_disable_irq(pn544_dev); */
	/* mt_eint_mask(CUST_EINT_IRQ_NFC_NUM); */

	i2c_set_clientdata(client, &_gpn544_dev);

	forceExitBlockingRead = 0;

	return 0;
}

static int pn544_remove(struct i2c_client *client)
{
	/* struct pn544_dev *pn544_dev; */

	pr_debug("pn544_remove\n");

#ifdef CONFIG_MTK_I2C_EXTENSION
	if (I2CDMAWriteBuf) {
		#ifdef CONFIG_64BIT
		dma_free_coherent(&client->dev, MAX_BUFFER_SIZE, I2CDMAWriteBuf,
				  I2CDMAWriteBuf_pa);
		#else
		dma_free_coherent(NULL, MAX_BUFFER_SIZE, I2CDMAWriteBuf,
				  I2CDMAWriteBuf_pa);
		#endif
		I2CDMAWriteBuf = NULL;
		I2CDMAWriteBuf_pa = 0;
	}

	if (I2CDMAReadBuf) {
		#ifdef CONFIG_64BIT
		dma_free_coherent(&client->dev, MAX_BUFFER_SIZE, I2CDMAReadBuf,
				  I2CDMAReadBuf_pa);
		#else
		dma_free_coherent(NULL, MAX_BUFFER_SIZE, I2CDMAReadBuf,
				  I2CDMAReadBuf_pa);
		#endif
		I2CDMAReadBuf = NULL;
		I2CDMAReadBuf_pa = 0;
	}
#endif

	/* pn544_dev = i2c_get_clientdata(client); */
	/* free_irq(client->irq, &_gpn544_dev);  */
	free_irq(nfc_irq, &_gpn544_dev);
	misc_deregister(&_gpn544_dev.pn544_device);
	mutex_destroy(&_gpn544_dev.read_mutex);

#if defined(CONFIG_MTK_LEGACY)
	gpio_free(_gpn544_dev.irq_gpio);
	gpio_free(_gpn544_dev.ven_gpio);
	gpio_free(_gpn544_dev.sysrstb_gpio);
	gpio_free(_gpn544_dev.eint_gpio);
#endif

	/* kfree(pn544_dev); */

	return 0;
}

static int mt_nfc_probe(struct platform_device *pdev)
{
	int ret = 0;

#if !defined(CONFIG_MTK_LEGACY)
	struct platform_device *nfc_plt_dev = NULL;

	nfc_plt_dev = pdev;

	pr_debug("%s : &nfc_plt_dev=%p\n", __func__, nfc_plt_dev);

	/* pinctrl init */
	ret = mt_nfc_pinctrl_init(pdev);

	/* gpio init */
	if (mt_nfc_gpio_init() != 0)
		pr_debug("%s : mt_nfc_gpio_init err.\n", __func__);

#endif

	return 0;
}

static int mt_nfc_remove(struct platform_device *pdev)
{
	pr_debug("%s : &pdev=%p\n", __func__, pdev);
	return 0;
}

/* void pn544_dev_irq_handler(void) */
irqreturn_t pn544_dev_irq_handler(int irq, void *data)
{
	struct pn544_dev *pn544_dev = pn544_dev_ptr;
	/* pr_debug("%s : &pn544_dev=%p\n", __func__, pn544_dev); */

	if (pn544_dev == NULL) {
		pr_debug("pn544_dev NULL.\n");
		return IRQ_HANDLED;
	}
	/* pn544_disable_irq(pn544_dev); */
	pn544_disable_irq(nfc_irq);

	wake_up(&pn544_dev->read_wq);
	/* wake_up_interruptible(&pn544_dev->read_wq); */

	/* pr_debug("%s : wake_up &read_wq=%p\n", __func__, &pn544_dev->read_wq); */

	return IRQ_HANDLED;
}

static ssize_t pn544_dev_read(struct file *filp, char __user *buf,
			       size_t count, loff_t *offset)
{
	struct pn544_dev *pn544_dev = filp->private_data;
	int ret = 0;
	int read_retry = 5;

	if (count > MAX_BUFFER_SIZE)
		count = MAX_BUFFER_SIZE;

	if (!mt_nfc_get_gpio_value(pn544_dev->irq_gpio)) {
		if (filp->f_flags & O_NONBLOCK) {
			ret = -EAGAIN;
			pr_debug("%s : goto fail.\n", __func__);
			goto fail;
		}
		/* mutex_lock(&pn544_dev->irq_enabled_lock); */
		/* pn544_dev->irq_enabled = true; */
		/* mutex_unlock(&pn544_dev->irq_enabled_lock); */

		mutex_lock(&pn544_dev->read_mutex);
		if (forceExitBlockingRead == 1) {
			pr_debug("%s :forceExitBlockingRead.\n", __func__);
			forceExitBlockingRead = 0;	/* clear flag */
			mutex_unlock(&pn544_dev->read_mutex);
			goto fail;
		}
		mutex_unlock(&pn544_dev->read_mutex);

		/* mt_eint_unmask(pn544_dev->client->irq);     */
		/* pr_debug("%s : mt_eint_unmask %d, IRQ, %d\n", */
		/*                __func__,   */
		/*                pn544_dev->client->irq, */
		/*                mt_get_gpio_in(pn544_dev->irq_gpio)); */

		pn544_enable_irq(nfc_irq);

		ret = wait_event_interruptible(pn544_dev->read_wq,
					       (mt_nfc_get_gpio_value
						(pn544_dev->irq_gpio)
						|| forceExitBlockingRead));

		if (ret || forceExitBlockingRead) {
			pn544_disable_irq(nfc_irq);
			pr_debug("%s : goto fail\n", __func__);
			mutex_lock(&pn544_dev->read_mutex);
			if (forceExitBlockingRead == 1) {
				pr_debug
				    ("%s:clear flag,orceExitBlockingRead\n",
				     __func__);
				forceExitBlockingRead = 0;	/* clear flag */
			}
			mutex_unlock(&pn544_dev->read_mutex);
			goto fail;
		}

	}
	#ifdef CONFIG_MTK_I2C_EXTENSION
	pn544_dev->client->addr = (pn544_dev->client->addr & I2C_MASK_FLAG);
	pn544_dev->client->ext_flag |= I2C_DMA_FLAG;
	/* pn544_dev->client->ext_flag |= I2C_DIRECTION_FLAG; */
	/* pn544_dev->client->ext_flag |= I2C_A_FILTER_MSG; */
	pn544_dev->client->timing = NFC_CLIENT_TIMING;

	/* Read data */
	ret =
	    i2c_master_recv(pn544_dev->client,
			    (unsigned char *)(uintptr_t) I2CDMAReadBuf_pa,
			    count);

	#else
	while (read_retry) {
		ret =
		    i2c_master_recv(pn544_dev->client,
				    (unsigned char *)(uintptr_t) I2CDMAReadBuf,
				    count);

		/* mutex_unlock(&pn544_dev->read_mutex); */

		if (ret < 0) {
			pr_debug("%s: i2c_master_recv failed: %d, read_retry: %d\n",
				__func__, ret, read_retry);
			read_retry--;
			usleep_range(900, 1000);
			continue;
		}
		break;
	}
	#endif
	if (ret < 0) {
		pr_err("%s: i2c_master_recv failed: %d, read_retry: %d\n",
			__func__, ret, read_retry);
		return ret;
	}

	if (ret > count) {
		pr_debug("%s: received too many bytes from i2c (%d)\n",
			 __func__, ret);
		return -EIO;
	}

	if (copy_to_user(buf, I2CDMAReadBuf, ret)) {
		pr_debug("%s : failed to copy to user space\n", __func__);
		return -EFAULT;
	}

	/* pr_debug("%s: return,ret,%d\n", __func__, ret); */

	return ret;

fail:
	/* mutex_unlock(&pn544_dev->read_mutex); */
	pr_debug("%s: return, fail: %d\n", __func__, ret);
	return ret;

}

static ssize_t pn544_dev_write(struct file *filp, const char __user *buf,
				size_t count, loff_t *offset)
{
	struct pn544_dev *pn544_dev;
	int ret = 0, ret_tmp = 0, count_ori = 0, count_remain = 0, idx = 0;

	pn544_dev = filp->private_data;
	count_ori = count;
	count_remain = count_ori;

	if (count > MAX_BUFFER_SIZE) {
		count = MAX_BUFFER_SIZE;
		count_remain -= count;
	}

	while (1) {
		if (copy_from_user
		    (I2CDMAWriteBuf, &buf[(idx * MAX_BUFFER_SIZE)], count)) {
			pr_debug("%s : failed to copy from user space.\n",
				 __func__);
			return -EFAULT;
		}

		/* Write data */
		#ifdef CONFIG_MTK_I2C_EXTENSION
		pn544_dev->client->addr =
		    (pn544_dev->client->addr & I2C_MASK_FLAG);

		pn544_dev->client->ext_flag |= I2C_DMA_FLAG;
		/* pn544_dev->client->ext_flag |= I2C_DIRECTION_FLAG; */
		/* pn544_dev->client->ext_flag |= I2C_A_FILTER_MSG; */
		pn544_dev->client->timing = NFC_CLIENT_TIMING;

		ret_tmp =
		    i2c_master_send(pn544_dev->client,
				    (unsigned char *)(uintptr_t)
				    I2CDMAWriteBuf_pa, count);
		#else
		ret_tmp =
		    i2c_master_send(pn544_dev->client,
				    (unsigned char *)(uintptr_t)
				    I2CDMAWriteBuf, count);
		#endif

		if (ret_tmp != count) {
			pr_debug("%s : i2c_master_send returned %d\n", __func__,
				 ret);
			ret = -EIO;
			return ret;
		}

		ret += ret_tmp;

		if (ret == count_ori) {
			/* pr_debug("%s : ret == count_ori\n", __func__); */
			break;
		}

			if (count_remain > MAX_BUFFER_SIZE) {
				count = MAX_BUFFER_SIZE;
				count_remain -= MAX_BUFFER_SIZE;
			} else {
				count = count_remain;
				count_remain = 0;
			}
			idx++;
		}

	return ret;
}

static int pn544_dev_open(struct inode *inode, struct file *filp)
{

	struct pn544_dev *pn544_dev =
	    container_of(filp->private_data, struct pn544_dev, pn544_device);

	filp->private_data = pn544_dev;
	pn544_dev_ptr = pn544_dev;

	pr_debug("pn544_dev_open,%s : %d,%d, &pn544_dev_open=%p\n", __func__,
		 imajor(inode), iminor(inode), pn544_dev_ptr);

	forceExitBlockingRead = 0;
	return 0;
}

static long pn544_dev_unlocked_ioctl(struct file *filp, unsigned int cmd,
				      unsigned long arg)
{

	struct pn544_dev *pn544_dev = filp->private_data;
	int result = 0;
	int gpio_dir, gpio_num = -1, tmp_gpio;

	if ((cmd & 0xFFFF) == 0xFE00) {

		pn544_disable_irq(nfc_irq);

		pr_debug("pn544_dev_unlocked_ioctl for disable_irq.\n");

		return 0;
	} else if ((cmd & 0xFFFF) == 0xFE01) {

		struct device_node *node;
		int ret;

#if !defined(CONFIG_MTK_LEGACY)
		mt_nfc_pinctrl_select(gpctrl, st_irq_init);
#else
		mt_set_gpio_mode(pn544_dev->irq_gpio, GPIO_IRQ_NFC_PIN_M_GPIO);
		mt_set_gpio_dir(pn544_dev->irq_gpio, GPIO_DIR_IN);
		mt_set_gpio_pull_enable(pn544_dev->irq_gpio, GPIO_PULL_ENABLE);
		mt_set_gpio_pull_select(pn544_dev->irq_gpio, GPIO_PULL_DOWN);
#endif

		/* mt_eint_set_hw_debounce(CUST_EINT_IRQ_NFC_NUM,  */
		/*                      CUST_EINT_IRQ_NFC_DEBOUNCE_CN); */
		/* mt_eint_registration(CUST_EINT_IRQ_NFC_NUM, */
		/*                      CUST_EINT_IRQ_NFC_TYPE, */
		/*                   pn544_dev_irq_handler, 0);  */
		/* mt_eint_mask(CUST_EINT_IRQ_NFC_NUM); */

		/*  NFC IRQ settings */
		node =
		    of_find_compatible_node(NULL, NULL,
					    "mediatek,nfc-gpio-v2");

		if (node) {

			nfc_irq = irq_of_parse_and_map(node, 0);

			/* client->irq = nfc_irq; */

			ret =
			    request_irq(nfc_irq, pn544_dev_irq_handler,
					IRQF_TRIGGER_NONE, "irq_nfc-eint",
					NULL);

			if (ret) {
				pr_err("%s : EINT IRQ LINE NOT AVAILABLE, ret = %d\n",
				       __func__, ret);
			} else {
				pr_debug
				    ("%s : set EINT finished, nfc_irq=%d.\n",
				     __func__, nfc_irq);
				nfc_irq_count++;
				pn544_disable_irq(nfc_irq);
				enable_irq_wake(nfc_irq);
			}
		} else {
			pr_err("%s : can not find NFC eint compatible node.\n",
			       __func__);
		}

		pr_debug("pn544_dev_unlocked_ioctl,Re-registered IRQ.\n");
		return 0;
	} else if ((cmd & 0xFFFF) == 0xFEFF) {	/* EXIT EINT */
		mutex_lock(&pn544_dev->read_mutex);
		forceExitBlockingRead = 1;
		mutex_unlock(&pn544_dev->read_mutex);

		mt_nfc_pinctrl_select(gpctrl, st_ven_h);
		mt_nfc_pinctrl_select(gpctrl, st_rst_l);
		pn544_disable_irq(nfc_irq);

		wake_up_interruptible(&pn544_dev->read_wq);
		pr_debug("pn544_dev_unlocked_ioctl,SW Release IRQ.\n");
		return 0;
	} else if ((cmd & 0xFFFF) == 0xFEFE) {	/* Get ChipID */
		return fgNfcChip;
	} else if ((cmd & 0xFFFF) == 0xFEFD) {
		fgNfcChip = (arg & 0xFFFF);
		return 0;
	}

	tmp_gpio = (((arg & 0xFF00) >> 8) & 0x00FF);

	if (tmp_gpio == MTK_NFC_GPIO_EN_B) {
		gpio_num = pn544_dev->ven_gpio;

	} else if (tmp_gpio == MTK_NFC_GPIO_SYSRST_B) {
		gpio_num = pn544_dev->sysrstb_gpio;

	} else if (tmp_gpio == MTK_NFC_GPIO_EINT) {
		gpio_num = pn544_dev->eint_gpio;

	} else if (tmp_gpio == MTK_NFC_GPIO_IRQ) {
		gpio_num = pn544_dev->irq_gpio;

	} else if (tmp_gpio == MTK_NFC_GPIO_IOCTL) {
		/*  IOCTL  */
		int command = (arg & 0x00FF);

		switch (command) {
		case MTK_NFC_IOCTL_CMD_CLOCK_BUF_ENABLE:
			pr_debug("%s, enable clock buffer.\n", __func__);
#ifndef CONFIG_MTK_FPGA
			/* enable nfc clock buffer */
			if (!is_clk_buf_from_pmic())
				clk_buf_ctrl(CLK_BUF_NFC, 1);
#endif
			break;
		case MTK_NFC_IOCTL_CMD_CLOCK_BUF_DISABLE:
			pr_debug("%s, disable clock buffer.\n", __func__);
#ifndef CONFIG_MTK_FPGA
			/* disable nfc clock buffer */
			if (!is_clk_buf_from_pmic())
				clk_buf_ctrl(CLK_BUF_NFC, 0);
#endif
			break;
		case MTK_NFC_IOCTL_CMD_EXIT_EINT:
			pr_debug("pn544_dev_unlocked_ioctl, EXIT EINT.\n");
			mutex_lock(&pn544_dev->read_mutex);
			forceExitBlockingRead = 1;
			mutex_unlock(&pn544_dev->read_mutex);
			wake_up_interruptible(&pn544_dev->read_wq);
			pr_debug("pn544_dev_unlocked_ioctl,SW Release IRQ\n");
			break;
		case MTK_NFC_IOCTL_CMD_GET_CHIP_ID:
			return fgNfcChip;
		case MTK_NFC_IOCTL_CMD_READ_DATA:
			pr_debug("Call pn544_dev_irq_handler. irq=%d.\n",
				 mt_nfc_get_gpio_value(pn544_dev->irq_gpio));
			if (mt_nfc_get_gpio_value(pn544_dev->irq_gpio))
				pn544_dev_irq_handler(nfc_irq, NULL);
			else
				pr_err("%s: get irq failed\n", __func__);
			break;
		default:
			break;
		}
		return 0;
	}

	if ((tmp_gpio != MTK_NFC_GPIO_EN_B) && (tmp_gpio != MTK_NFC_GPIO_SYSRST_B)
	    && (tmp_gpio != MTK_NFC_GPIO_EINT) && (tmp_gpio != MTK_NFC_GPIO_IRQ)) {
		result = MTK_NFC_PULL_INVALID;
		pr_debug("%s, invalid ioctl.\n", __func__);
		return result;
	}

	if (-1 == gpio_num)
		return 0;

	/* if (result != MTK_NFC_PULL_INVALID) { */
	if (cmd == MTK_NFC_IOCTL_READ) {
		result = mt_nfc_get_gpio_value(gpio_num);

		pr_debug("%s : get gpio value: %d, gpio_num: %d\n", __func__, result, gpio_num);

			/*error handler for eint_registration abnormal case */
			if (tmp_gpio == MTK_NFC_GPIO_IRQ && result == 0x01) {
				pr_debug
				    ("%s,irq=igh call pn544_dev_irq_handler\n",
				     __func__);
				pn544_dev_irq_handler(nfc_irq, NULL);
			}

	} else if (cmd == MTK_NFC_IOCTL_WRITE) {
		gpio_dir = mt_nfc_get_gpio_dir(gpio_num);

		if (gpio_dir == MTK_NFC_GPIO_DIR_OUT) {
			int gpio_pol = (arg & 0x00FF);

			if (gpio_pol == MTK_NFC_PULL_LOW) {
#if !defined(CONFIG_MTK_LEGACY)
				switch (tmp_gpio) {
				case MTK_NFC_GPIO_EN_B:
					result =
					    mt_nfc_pinctrl_select(gpctrl,
								  st_ven_l);
					break;

				case MTK_NFC_GPIO_SYSRST_B:
					result =
					    mt_nfc_pinctrl_select(gpctrl,
								  st_rst_l);
					break;

				case MTK_NFC_GPIO_EINT:
					result =
					    mt_nfc_pinctrl_select(gpctrl,
								  st_eint_l);
					break;

				case MTK_NFC_GPIO_IRQ:
					pr_debug
					    ("%s : IRQ cannot set.\n",
					     __func__);
					break;
				default:
					pr_debug("%s : default case.\n",
						 __func__);
					break;
				}
#else
					result =
				    mt_set_gpio_out(gpio_num, GPIO_OUT_ZERO);
				/* pr_debug("%s : call mtk legacy.\n", __func__); */
#endif
			} else if (gpio_pol == MTK_NFC_PULL_HIGH) {
#if !defined(CONFIG_MTK_LEGACY)
				switch (tmp_gpio) {
				case MTK_NFC_GPIO_EN_B:
					result =
					    mt_nfc_pinctrl_select(gpctrl,
								  st_ven_h);
					break;

				case MTK_NFC_GPIO_SYSRST_B:
					result =
					    mt_nfc_pinctrl_select(gpctrl,
								  st_rst_h);
					break;

				case MTK_NFC_GPIO_EINT:
					result =
					    mt_nfc_pinctrl_select(gpctrl,
								  st_eint_h);
					break;

				case MTK_NFC_GPIO_IRQ:
					pr_debug
					    ("%s : IRQ cannot pull.\n",
					     __func__);
					break;
				default:
					pr_debug("%s : default case.\n",
						 __func__);
					break;
				}
#else
					result =
				    mt_set_gpio_out(gpio_num, GPIO_OUT_ONE);
				/* pr_debug("%s : call mtk legacy.\n", __func__); */
#endif
				}
			} else {
				result = MTK_NFC_PULL_INVALID;
			}
		} else {
			result = MTK_NFC_PULL_INVALID;
		}
	/* } */
	return result;
}

#if !defined(CONFIG_MTK_LEGACY)
static int mt_nfc_pinctrl_select(struct pinctrl *p, struct pinctrl_state *s)
{
	int ret = 0;

	if (p != NULL && s != NULL) {
		ret = pinctrl_select_state(p, s);
	} else {
		pr_debug("%s : pinctrl_select err\n", __func__);
		ret = -1;
	}
	return ret;
}

static int mt_nfc_pinctrl_init(struct platform_device *pdev)
{
	int ret = 0;

	gpctrl = devm_pinctrl_get(&pdev->dev);
	if (IS_ERR(gpctrl)) {
		dev_err(&pdev->dev, "Cannot find pinctrl!");
		ret = PTR_ERR(gpctrl);
		goto end;
	}

	st_ven_h = pinctrl_lookup_state(gpctrl, "ven_high");
	if (IS_ERR(st_ven_h)) {
		ret = PTR_ERR(st_ven_h);
		pr_debug("%s : pinctrl err, ven_high\n", __func__);
	}
	if (st_ven_h == NULL) {
		pr_err("%s : st_ven_h is NULL\n", __func__);
		goto end;
	}

	st_ven_l = pinctrl_lookup_state(gpctrl, "ven_low");
	if (IS_ERR(st_ven_l)) {
		ret = PTR_ERR(st_ven_l);
		pr_debug("%s : pinctrl err, ven_low\n", __func__);
	}
	if (st_ven_l == NULL) {
		pr_err("%s : st_ven_l is NULL\n", __func__);
		goto end;
	}

	st_rst_h = pinctrl_lookup_state(gpctrl, "rst_high");
	if (IS_ERR(st_rst_h)) {
		ret = PTR_ERR(st_rst_h);
		pr_debug("%s : pinctrl err, rst_high\n", __func__);
	}
	if (st_rst_h == NULL) {
		pr_err("%s : st_rst_h is NULL\n", __func__);
		goto end;
	}

	st_rst_l = pinctrl_lookup_state(gpctrl, "rst_low");
	if (IS_ERR(st_rst_l)) {
		ret = PTR_ERR(st_rst_l);
		pr_debug("%s : pinctrl err, rst_low\n", __func__);
	}
	if (st_rst_l == NULL) {
		pr_err("%s : st_rst_l is NULL\n", __func__);
		goto end;
	}

	st_eint_h = pinctrl_lookup_state(gpctrl, "eint_high");
	if (IS_ERR(st_eint_h)) {
		ret = PTR_ERR(st_eint_h);
		pr_debug("%s : pinctrl err, eint_high\n", __func__);
	}
	if (st_eint_h == NULL) {
		pr_err("%s : st_eint_h is NULL\n", __func__);
		goto end;
	}

	st_eint_l = pinctrl_lookup_state(gpctrl, "eint_low");
	if (IS_ERR(st_eint_l)) {
		ret = PTR_ERR(st_eint_l);
		pr_debug("%s : pinctrl err, eint_low\n", __func__);
	}
	if (st_eint_l == NULL) {
		pr_err("%s : st_eint_l is NULL\n", __func__);
		goto end;
	}

	st_irq_init = pinctrl_lookup_state(gpctrl, "irq_init");
	if (IS_ERR(st_irq_init)) {
		ret = PTR_ERR(st_irq_init);
		pr_debug("%s : pinctrl err, irq_init\n", __func__);
	}
	if (st_irq_init == NULL) {
		pr_err("%s : st_irq_init is NULL\n", __func__);
		goto end;
	}
#if 0
	st_osc_init = pinctrl_lookup_state(gpctrl, "osc_init");
	if (IS_ERR(st_osc_init)) {
		ret = PTR_ERR(st_osc_init);
		pr_debug("%s : pinctrl err, osc_init\n", __func__);
	}
	if (st_osc_init == NULL) {
		pr_err("%s : st_osc_init is NULL\n", __func__);
		goto end;
	}
#endif
	/* select state */
#if 0
	ret = mt_nfc_pinctrl_select(gpctrl, st_osc_init);
	usleep_range(900, 1000);
#endif
	ret = mt_nfc_pinctrl_select(gpctrl, st_irq_init);
	usleep_range(900, 1000);

	ret = mt_nfc_pinctrl_select(gpctrl, st_ven_h);
	usleep_range(900, 1000);

	ret = mt_nfc_pinctrl_select(gpctrl, st_rst_h);
	usleep_range(900, 1000);

	ret = mt_nfc_pinctrl_select(gpctrl, st_eint_l);

end:

	return ret;
}

static int mt_nfc_gpio_init(void)
{
	struct device_node *node;

	node = of_find_compatible_node(NULL, NULL, "mediatek,nfc-gpio-v2");

	if (node) {
		pn544_platform_data.ven_gpio = of_get_named_gpio(node, "gpio-ven", 0);
		pn544_platform_data.sysrstb_gpio = of_get_named_gpio(node, "gpio-rst", 0);
		pn544_platform_data.eint_gpio = of_get_named_gpio(node, "gpio-eint", 0);
		pn544_platform_data.irq_gpio = of_get_named_gpio(node, "gpio-irq", 0);
	} else {
		pr_debug("%s : get gpio num err.\n", __func__);
		return -1;
	}

	_gpn544_dev.irq_gpio = pn544_platform_data.irq_gpio;
	_gpn544_dev.ven_gpio = pn544_platform_data.ven_gpio;
	_gpn544_dev.sysrstb_gpio = pn544_platform_data.sysrstb_gpio;
	_gpn544_dev.eint_gpio = pn544_platform_data.eint_gpio;

	return 0;
}
#endif

static int mt_nfc_get_gpio_value(int gpio_num)
{
	int value = 0;

	if (mt_nfc_get_gpio_dir(gpio_num) != MTK_NFC_GPIO_DIR_INVALID) {

#if !defined(CONFIG_MTK_LEGACY)
		value = __gpio_get_value(gpio_num);
#else
		value = mt_get_gpio_in(gpio_num);
#endif

	}

	return value;
}

static int mt_nfc_get_gpio_dir(int gpio_num)
{
	if (gpio_num == pn544_platform_data.irq_gpio) {
		return MTK_NFC_GPIO_DIR_IN;	/* input */

	} else if ((gpio_num == pn544_platform_data.ven_gpio) ||
		   (gpio_num == pn544_platform_data.sysrstb_gpio) ||
		   (gpio_num == pn544_platform_data.eint_gpio)) {
		return MTK_NFC_GPIO_DIR_OUT;	/* output */

	} else {
		return MTK_NFC_GPIO_DIR_INVALID;

	}
}

/* return 0, success; return <0, fail
 * md_id        : modem id
 * md_state   : 0, on ; 1, off ;
 * vsim_state : 0, on ; 1, off;
 */
int inform_nfc_vsim_change(int md_id, int md_state, int vsim_state)
{
	char send_data[] = {
		0xaa, 0x0f, 0x03, 0x00, 0x03,
		0x00, 0x00, 0xaa, 0xf0
	};
	int ret = 0;
	int send_bytes = sizeof(send_data);
	int retry;

	/* AA 0F 03 00 03 00 xx AA F0   */
	/* where xx is defined:  */
	/* Bit[7:4] : md_id  */
	/* Bit[3:2] : md_state */
	/* Bit[1:0] : vsim_state */

	send_data[6] |= (md_id << 4);
	send_data[6] |= (md_state << 2);
	send_data[6] |= (vsim_state);

	pr_debug
	    ("%s, md_id,%d, md_state,%d, vsim_state,%d , send_data[6],0x%X.\n",
	     __func__, md_id, md_state, vsim_state, send_data[6]);

	/* send to pn544 */
	#ifdef CONFIG_MTK_I2C_EXTENSION
	_gpn544_dev.client->addr = (_gpn544_dev.client->addr & I2C_MASK_FLAG);
	_gpn544_dev.client->ext_flag |= I2C_DMA_FLAG;
	_gpn544_dev.client->timing = 400;
	#endif

	memcpy(I2CDMAWriteBuf, send_data, send_bytes);

	/* eint pull high */
#if !defined(CONFIG_MTK_LEGACY)
	mt_nfc_pinctrl_select(gpctrl, st_eint_h);
#else
	ret = mt_set_gpio_out(pn544_platform_data.eint_gpio, GPIO_OUT_ONE);
#endif

	/* sleep 5ms */
	/* msleep(5); */
	usleep_range(1800, 2000);

	for (retry = 0; retry < 10; retry++) {
		/* ret = i2c_master_send(_gpn544_dev.client,  */
		/*      (unsigned char *)I2CDMAWriteBuf_pa, send_bytes); */

		#ifdef CONFIG_MTK_I2C_EXTENSION
		ret =
		    i2c_master_send(_gpn544_dev.client,
				    (unsigned char *)(uintptr_t)
				    I2CDMAWriteBuf_pa, send_bytes);
		#else
		ret =
		    i2c_master_send(_gpn544_dev.client,
				    (unsigned char *)(uintptr_t)
				    I2CDMAWriteBuf, send_bytes);
		#endif

		if (ret == send_bytes) {
			pr_debug
			    ("%s, send to pn544 OK. retry %d.\n",
			     __func__, retry);
			break;
		}
			pr_debug
			    ("%s, send to pn544 fail. retry %d, ret %d.\n",
			     __func__, retry, ret);
		/* sleep 2ms */
		/* msleep(2); */
		usleep_range(1800, 2000);
	}
	/* eint pull low */

#if !defined(CONFIG_MTK_LEGACY)
	mt_nfc_pinctrl_select(gpctrl, st_eint_l);
#else
	ret = mt_set_gpio_out(pn544_platform_data.eint_gpio, GPIO_OUT_ZERO);
#endif
	return 0;
}

/*
 * module load/unload record keeping
 */
static int __init pn544_dev_init(void)
{
	int ret;

	pr_debug("pn544_dev_init\n");
	/*  i2c_register_board_info(NFC_I2C_BUSNUM, &nfc_board_info, 1); */
	/*  pr_debug("pn544_dev_init2\n"); */

	platform_driver_register(&mtk_nfc_platform_driver);

	i2c_add_driver(&pn544_dev_driver);

	_gpn544_dev.pn544_device.minor = MISC_DYNAMIC_MINOR;
	_gpn544_dev.pn544_device.name = "pn544";
	_gpn544_dev.pn544_device.fops = &pn544_dev_fops;

	ret = misc_register(&_gpn544_dev.pn544_device);
	if (ret) {
		pr_debug("%s : misc_register failed\n", __FILE__);
		return ret;
	}

	pr_debug("pn544_dev_init success\n");
	return 0;
}

static void __exit pn544_dev_exit(void)
{
	pr_debug("pn544_dev_exit\n");

	i2c_del_driver(&pn544_dev_driver);
}

module_init(pn544_dev_init);
module_exit(pn544_dev_exit);

MODULE_AUTHOR("LiangChi Huang");
MODULE_DESCRIPTION("MTK NFC driver");
MODULE_LICENSE("GPL");
