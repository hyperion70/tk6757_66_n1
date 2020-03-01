/*
 * Reversed and adapted for Santin N1
 * by zoggn@HakonTI 2020y
 *
 */

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
#include <linux/miscdevice.h>
#include <linux/spinlock.h>
#include <linux/dma-mapping.h>
#include <mtk_gpio.h>


//Why | 0x80000000 here ? because if do not do this, mt_gpio_pin_decrypt will dump_stack~
// ISNT CORRECT WAY TO SET GPIO!!! NEED TO GET IT FROM DTS 
#define VEN_PIN         	(28 | 0x80000000)
#define RST_PIN       		(25 | 0x80000000)
#define IRQ_PIN         	(26 | 0x80000000)
#define PN544_DRVNAME		"pn544"

#define MAX_BUFFER_SIZE		512

#define pn547_MAGIC	0xE9

/*
 * pn547 power control via ioctl
 * pn547_SET_PWR(0): power off
 * pn547_SET_PWR(1): power on
 * pn547_SET_PWR(2): reset and power on with firmware download enabled
 */
#define PN544_SET_PWR	_IOW(pn547_MAGIC, 0x01, unsigned int)

struct pn544_dev	{
	wait_queue_head_t	read_wq;
	struct mutex		read_mutex;
	struct i2c_client	*client;
	struct miscdevice	pn544_device;
	bool			irq_enabled;
	spinlock_t		irq_enabled_lock;
};

static unsigned int pn547_irq;

static struct pn544_dev *p_pn544_dev = NULL;
//For DMA
static char I2CDMAWriteBuf[MAX_BUFFER_SIZE];
static char I2CDMAReadBuf[MAX_BUFFER_SIZE];
static int nfc_exist=0;

struct platform_device nfc_pn547 = {
	.name	       = PN544_DRVNAME,
	.id            = -1,
};
/*----------------------------------------------------------------------------*/

#if 0 // just compatibility
static ssize_t nfc_show(struct device *dev, 
                                  struct device_attribute *attr, char *buf)
{	
    printk("nfc test: nfc show function!\n");

    if (!dev ) {
       
        return 0;
    }
    printk("The nfc is pn547 \n");
    return snprintf(buf, PAGE_SIZE, "%d\n", nfc_exist);    
   //Lightsensor flag,Let the APP know the Psensor is CTP type or physical type// add by llf
}
/*----------------------------------------------------------------------------*/
static ssize_t nfc_store(struct device* dev, 
                                   struct device_attribute *attr, const char *buf, size_t count)
{

	return count;
}   
/*----------------------------------------------------------------------------*/
DEVICE_ATTR(nfc,     S_IWUSR | S_IRUGO, nfc_show, nfc_store);
/*----------------------------------------------------------------------------*/
#endif

static ssize_t show_pn547_info(struct device *dev, 
                                  struct device_attribute *attr, char *buf)
{
	int len = 0;
	printk("%s\n", __func__);
	if(nfc_exist){
		len = snprintf(buf, PAGE_SIZE, "%s:%s:%s:%s\n", "pn547", "NXP", "sup1", "1");
	}else{
		len = snprintf(buf, PAGE_SIZE, "%s:%s:%s:%s\n", "NA", "NA", "NA", "NA");
	}
	return len;
}
DEVICE_ATTR(nfc, S_IRUGO, show_pn547_info, NULL);


static void pn544_disable_irq(struct pn544_dev *pn544_dev)
{
	unsigned long flags;


	spin_lock_irqsave(&pn544_dev->irq_enabled_lock, flags);
	if (pn544_dev->irq_enabled) {
       		disable_irq_nosync(pn547_irq);
		pn544_dev->irq_enabled = false;
	}
	spin_unlock_irqrestore(&pn544_dev->irq_enabled_lock, flags);


}

static irqreturn_t  pn544_dev_irq_handler(int irq, void *dev_id)
{
	struct pn544_dev *pn544_dev = p_pn544_dev;

	if (!mt_get_gpio_in(IRQ_PIN)) {

		printk(KERN_DEBUG "***************\n");
		
		return IRQ_HANDLED;
	}


	pn544_disable_irq(pn544_dev);

	/* Wake up waiting readers */
	wake_up(&pn544_dev->read_wq);

        return IRQ_HANDLED;
}


static ssize_t pn544_dev_read(struct file *filp, char __user *buf, size_t count, loff_t *offset)
{
	struct pn544_dev *pn544_dev = filp->private_data;
	int ret,i;
//	char tmp[MAX_BUFFER_SIZE];

	if (count > MAX_BUFFER_SIZE)
		count = MAX_BUFFER_SIZE;

	printk("pn544 %s : reading %zu bytes.\n", __func__, count);

	mutex_lock(&pn544_dev->read_mutex);

	if (!mt_get_gpio_in(IRQ_PIN)) {
        
		printk("pn544 read no event\n");
		
		if (filp->f_flags & O_NONBLOCK) {
			ret = -EAGAIN;
			goto fail;
		}
		
		printk("pn544 read wait event\n");
		
		pn544_dev->irq_enabled = true;

		enable_irq(pn547_irq);
	
		ret = wait_event_interruptible(pn544_dev->read_wq, mt_get_gpio_in(IRQ_PIN));

		pn544_disable_irq(pn544_dev);

		if (ret) {
			printk("pn544 read wait event error\n");
			goto fail;
		}
	}

	/* Read data */
	
    ret = i2c_master_recv(pn544_dev->client,  (unsigned char *)(uintptr_t)I2CDMAReadBuf, count);
	   
	mutex_unlock(&pn544_dev->read_mutex);


	if (ret < 0) {
		pr_err("pn544 %s: i2c_master_recv returned %d\n", __func__, ret);
		return ret;
	}
	if (ret > count) {
		pr_err("pn544 %s: received too many bytes from i2c (%d)\n", __func__, ret);
		return -EIO;
	}
	
   if (copy_to_user(buf, I2CDMAReadBuf, ret)) 
   {
      printk(KERN_DEBUG "%s : failed to copy to user space\n", __func__);
      return -EFAULT;
   }

	printk("pn544 IFD->PC:");
	for(i = 0; i < ret; i++) {
		printk(" %02X", I2CDMAReadBuf[i]);
	}
	printk("\n");

	return ret;

fail:
	mutex_unlock(&pn544_dev->read_mutex);
	return ret;
}

static ssize_t pn544_dev_write(struct file *filp, const char __user *buf, size_t count, loff_t *offset) 
{
	struct pn544_dev *pn544_dev;
	int ret, i,idx = 0;

	pn544_dev = p_pn544_dev;

	if (count > MAX_BUFFER_SIZE)
		count = MAX_BUFFER_SIZE;

	if (copy_from_user(I2CDMAWriteBuf, &buf[(idx*255)], count)) 
	{
		printk(KERN_DEBUG "%s : failed to copy from user space\n", __func__);
		return -EFAULT;
	}
	printk("pn544 %s : writing %zu bytes.\n", __func__, count);
	/* Write data */
      ret = i2c_master_send(pn544_dev->client,  (unsigned char *)(uintptr_t)I2CDMAWriteBuf, count);
	if (ret != count) {
		pr_err("pn544 %s : i2c_master_send returned %d\n", __func__, ret);
		ret = -EIO;
	}
	printk("pn544 PC->IFD:");
	for(i = 0; i < count; i++) {
		printk(" %02X\n", I2CDMAWriteBuf[i]);
	}
	printk("\n");
	

	return ret;
}

static int pn544_dev_open(struct inode *inode, struct file *filp) 
{
	struct pn544_dev *pn544_dev = container_of(filp->private_data,
						struct pn544_dev,
						pn544_device);

	filp->private_data = pn544_dev;
	
	pr_debug("pn544 %s : %d,%d\n", __func__, imajor(inode), iminor(inode));

	return 0;
}

static long pn544_dev_ioctl(struct file *filp, unsigned int cmd, unsigned long arg) 
{
	switch (cmd) {
		case PN544_SET_PWR:
			if (arg == 2) {
				/* power on with firmware download (requires hw reset)
				 */
				printk("pn544 %s power on with firmware\n", __func__);
				mt_set_gpio_out(VEN_PIN, 1);
				printk(" ppn544_dev_ioctl_VEN_PIN=%d\n",mt_get_gpio_out(VEN_PIN));
				mt_set_gpio_out(RST_PIN, 1);
				printk(" ppn544_dev_ioctl_RST_PIN=%d\n",mt_get_gpio_out(RST_PIN));
				msleep(10);
				mt_set_gpio_out(VEN_PIN, 0);
				printk(" ppn544_dev_ioctl_VEN_PIN=%d\n",mt_get_gpio_out(VEN_PIN));
				msleep(50);
				mt_set_gpio_out(VEN_PIN, 1);
				printk(" ppn544_dev_ioctl_VEN_PIN=%d\n",mt_get_gpio_out(VEN_PIN));
				msleep(100);
			} else if (arg == 1) {
				/* power on */
				printk("pn544 %s power on\n", __func__);
				mt_set_gpio_out(RST_PIN, 0);
				mt_set_gpio_out(VEN_PIN, 1);
				msleep(100);
			} else  if (arg == 0) {
				/* power off */
				printk("pn544 %s power off\n", __func__);
				mt_set_gpio_out(RST_PIN, 0);
				mt_set_gpio_out(VEN_PIN, 0);
				msleep(50);
			} else {
				printk("pn544 %s bad arg %lu\n", __func__, arg);
				return -EINVAL;
			}
			break;
		default:
			printk("pn544 %s bad ioctl %u\n", __func__, cmd);
			return -EINVAL;
	}

	return 0;
}

static const struct file_operations pn544_dev_fops = {
	.owner	= THIS_MODULE,
	.llseek	= no_llseek,
	.read	= pn544_dev_read,
	.write	= pn544_dev_write,
	.open	= pn544_dev_open,
	.unlocked_ioctl  = pn544_dev_ioctl,
};


extern unsigned int irq_of_parse_and_map(struct device_node *dev, int index);

static int pn544_probe(struct i2c_client *client, const struct i2c_device_id *id) 
{
	int ret;
	struct pn544_dev *pn544_dev;
	struct device_node *node;
    
    printk("ZoggnINC 2020. PN544_SANTIN.\n");
    
	printk("pn544 nfc probe step01 is ok\n");

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		pr_err("pn544 %s : need I2C_FUNC_I2C\n", __func__);
		return  -ENODEV;
	}

	printk("pn544 nfc probe step02 is ok\n");

	pn544_dev = kzalloc(sizeof(*pn544_dev), GFP_KERNEL);
	if (pn544_dev == NULL) {
		dev_err(&client->dev, "pn544 failed to allocate memory for module data\n");
		return -ENOMEM;
	}
	memset(pn544_dev, 0, sizeof(struct pn544_dev));
	p_pn544_dev = pn544_dev;

	printk("pn544 nfc probe step03 is ok\n");
	
	client->addr = (client->addr | 0x08);
    printk("pn544 client->addr = %x\n", client->addr);
	pn544_dev->client = client;

	/* init mutex and queues */
	init_waitqueue_head(&pn544_dev->read_wq);
	mutex_init(&pn544_dev->read_mutex);
	spin_lock_init(&pn544_dev->irq_enabled_lock);

	pn544_dev->pn544_device.minor = MISC_DYNAMIC_MINOR;
	pn544_dev->pn544_device.name = PN544_DRVNAME;
	pn544_dev->pn544_device.fops = &pn544_dev_fops;

	ret = misc_register(&pn544_dev->pn544_device);
	if (ret) {
		pr_err("pn544 %s : misc_register failed\n", __FILE__);
		goto err_misc_register;
	}
    
	printk("pn544 nfc probe step04 is ok\n");
    
	/* VEN */
	mt_set_gpio_mode(VEN_PIN, GPIO_MODE_00);
	mt_set_gpio_dir(VEN_PIN, GPIO_DIR_OUT);
    
	/* GPIO4 */
	mt_set_gpio_mode(RST_PIN, GPIO_MODE_00);
	mt_set_gpio_dir(RST_PIN, GPIO_DIR_OUT);
    
    mt_set_gpio_mode(IRQ_PIN, GPIO_MODE_00);
	mt_set_gpio_dir(IRQ_PIN, GPIO_DIR_IN);
	mt_set_gpio_pull_enable(IRQ_PIN, GPIO_PULL_ENABLE);
	mt_set_gpio_pull_select(IRQ_PIN, GPIO_PULL_DOWN);
    
	printk("pn544 nfc probe step05 is ok\n");
    
	pn544_dev->irq_enabled = true;

    node = of_find_compatible_node(NULL, NULL, "mediatek, irq_nfc-eint");

	if (node) {

		pn547_irq = irq_of_parse_and_map(node, 0);

		client->irq = pn547_irq;

                ret = request_irq(pn547_irq,  pn544_dev_irq_handler, IRQF_TRIGGER_NONE, "irq_nfc-eint", NULL);

	if (ret) {

              ret = -1;
              printk("%s : requesting IRQ error\n", __func__);

	} 

	} else {
		printk("%s : can not find NFC eint compatible node\n",  __func__);
	}

	
	pn544_disable_irq(pn544_dev);


	printk("%s : requesting IRQ %d\n", __func__, client->irq);
	i2c_set_clientdata(client, pn544_dev);
	
	printk("pn544 nfc probe step06 is ok\n");


	return 0;

err_misc_register:
	mutex_destroy(&pn544_dev->read_mutex);
	kfree(pn544_dev);
	p_pn544_dev = NULL;  
	return ret;
}

static int pn544_remove(struct i2c_client *client)
{
	struct pn544_dev *pn544_dev;

	pn544_dev = i2c_get_clientdata(client);
	misc_deregister(&pn544_dev->pn544_device);
	mutex_destroy(&pn544_dev->read_mutex);
	kfree(pn544_dev);
	p_pn544_dev = NULL;
	
	return 0;
}

static const struct i2c_device_id pn544_id[] = { { PN544_DRVNAME, 0 }, {} };
MODULE_DEVICE_TABLE(i2c, pn544_id);

static const struct of_device_id nfc_switch_of_match[] = {{.compatible = "mediatek,NFC"}, {}, };

static int pn544_detect(struct i2c_client *client, struct i2c_board_info *info) 
{         
	printk(" pn544_detect \n");
	strcpy(info->type, PN544_DRVNAME);
	return 0;
}

static struct i2c_driver pn544_driver = {
	.id_table	= pn544_id,
	.probe		= pn544_probe,
	.remove		= pn544_remove,
	.detect     = pn544_detect,
	.driver		= {
		.owner	= THIS_MODULE,
		.name	= PN544_DRVNAME,
		.of_match_table = nfc_switch_of_match,
	},
};

/*
 * module load/unload record keeping
 */
 static int pn544_platform_probe(struct platform_device *pdev) 
{
         int ret ;
	 printk("add pn544_driver\n");
	 
	 ret = i2c_add_driver(&pn544_driver);
	 if(ret !=0)
        {
            printk("add pn544_driver fail\n");
            return -1;
        }	
	 return 0;
}
 static int pn544_platform_remove(struct platform_device *pdev) 
{
	 return 0;
}

static struct platform_driver pn544_platform_driver = {
	.probe      = pn544_platform_probe,
	.remove     = pn544_platform_remove,    
	.driver     = 
	{
		.name  = PN544_DRVNAME,
		.owner = THIS_MODULE,
	}
};
static int __init pn544_dev_init(void)
{
        int ret;
		
	printk("pn544_dev_init\n");
	
	ret = platform_device_register(&nfc_pn547);
	if( ret){
		printk("platform_device_register pn544 failed!\n");
		return ret;
	}
	
	if(platform_driver_register(&pn544_platform_driver)){
		printk("platform_driver_register failed\n");
                return -ENODEV;
	}
	return 0;
}
module_init(pn544_dev_init);

static void __exit pn544_dev_exit(void)
{
	printk("pn544 Unloading pn544 driver\n");
	i2c_del_driver(&pn544_driver);
}
module_exit(pn544_dev_exit);

MODULE_AUTHOR("XXX");
MODULE_DESCRIPTION("NFC PN544 driver");
MODULE_LICENSE("GPL");
