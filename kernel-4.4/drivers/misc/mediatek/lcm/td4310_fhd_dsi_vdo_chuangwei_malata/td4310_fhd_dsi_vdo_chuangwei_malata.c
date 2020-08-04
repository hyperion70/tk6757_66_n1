/*
 * Reversed by zoggn for Santin N1
 */

#define LOG_TAG "LCM"

#ifndef BUILD_LK
#include <linux/string.h>
#include <linux/kernel.h>
#endif

#include "lcm_drv.h"


#ifdef BUILD_LK
#include <platform/upmu_common.h>
#include <platform/mt_gpio.h>
#include <platform/mt_i2c.h>
#include <platform/mt_pmic.h>
#include <string.h>
#elif defined(BUILD_UBOOT)
#include <asm/arch/mt_gpio.h>
#else
#include <linux/regulator/mediatek/mtk_regulator.h>
#endif

#ifdef BUILD_LK
#define LCM_LOGI(string, args...)  dprintf(0, "[LK/"LOG_TAG"]"string, ##args)
#define LCM_LOGD(string, args...)  dprintf(1, "[LK/"LOG_TAG"]"string, ##args)
#else
#define LCM_LOGI(fmt, args...)  pr_debug("[KERNEL/"LOG_TAG"]"fmt, ##args)
#define LCM_LOGD(fmt, args...)  pr_debug("[KERNEL/"LOG_TAG"]"fmt, ##args)
#endif

static const unsigned int BL_MIN_LEVEL = 20;
static LCM_UTIL_FUNCS lcm_util;


#define SET_RESET_PIN(v)	(lcm_util.set_reset_pin((v)))
#define MDELAY(n)		(lcm_util.mdelay(n))
#define UDELAY(n)		(lcm_util.udelay(n))

#define dsi_set_cmdq_V22(cmdq, cmd, count, ppara, force_update) \
lcm_util.dsi_set_cmdq_V22(cmdq, cmd, count, ppara, force_update)
#define dsi_set_cmdq_V2(cmd, count, ppara, force_update) \
lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update) \
lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd) lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums) \
lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd) \
lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size) \
lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)

#ifndef BUILD_LK
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/i2c.h>
#include <linux/irq.h>
/* #include <linux/jiffies.h> */
/* #include <linux/delay.h> */
#include <linux/uaccess.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#endif

#define FRAME_WIDTH										(1080)
#define FRAME_HEIGHT									(1920)

#define REGFLAG_DELAY		0xFFFC
#define REGFLAG_UDELAY	0xFFFB
#define REGFLAG_END_OF_TABLE	0xFFFD

static LCM_DSI_MODE_SWITCH_CMD lcm_switch_mode_cmd;

struct LCM_setting_table {
    unsigned int cmd;
    unsigned char count;
    unsigned char para_list[64];
};

static struct LCM_setting_table lcm_initialization_setting[] =
{
    { 0x11, 0x01, {0x00}},
    { REGFLAG_DELAY, 150, {0x00}},
    { 0x29, 0x01, {0x00}},
    { REGFLAG_DELAY, 20, {0x00}},
    { REGFLAG_END_OF_TABLE, 0x00, {0x00}}
};

static struct LCM_setting_table lcm_suspend_setting[] = {
    {0x28, 0, {} },
    {REGFLAG_DELAY, 20, {} },
    {0x10, 0, {} },
    {REGFLAG_DELAY, 120, {} }
};

static struct LCM_setting_table bl_level[] = {
    {0x51, 1, {0xFF} },
    {REGFLAG_END_OF_TABLE, 0x00, {} }
};

static void push_table(void *cmdq, struct LCM_setting_table *table,
                       unsigned int count, unsigned char force_update)
{
    unsigned int i;
    unsigned cmd;
    
    for (i = 0; i < count; i++) {
        cmd = table[i].cmd;
        
        switch (cmd) {
            
            case REGFLAG_DELAY:
                if (table[i].count <= 10)
                    MDELAY(table[i].count);
                else
                    MDELAY(table[i].count);
                break;
                
            case REGFLAG_UDELAY:
                UDELAY(table[i].count);
                break;
                
            case REGFLAG_END_OF_TABLE:
                break;
                
            default:
                dsi_set_cmdq_V22(cmdq, cmd, table[i].count, table[i].para_list, force_update);
        }
    }
}


static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}


static void lcm_get_params(LCM_PARAMS *params)
{
    memset(params, 0, sizeof(LCM_PARAMS));
    
    params->type = LCM_TYPE_DSI;
    
    params->type = 2;
    params->dsi.mode = 2;
    params->dsi.data_format.format = 2;
    params->dsi.PS = 2;
    params->dsi.vertical_sync_active = 2;
    params->dsi.vertical_frontporch = 24;
    params->dsi.PLL_CLOCK = 468;
    params->width = FRAME_WIDTH;
    params->dsi.lcm_esd_check_table[0].cmd = 10;
    params->height = FRAME_HEIGHT;
    params->lcm_if = 1;
    params->dsi.LANE_NUM = 4;
    params->dsi.packet_size = 256;
    params->dsi.vertical_backporch = 4;
    params->dsi.vertical_active_line = 1920;
    params->dsi.horizontal_sync_active = 4;
    params->dsi.horizontal_backporch = 100;
    params->dsi.horizontal_frontporch = 100;
    params->dsi.horizontal_active_pixel = 1080;
    params->dsi.esd_check_enable = 0;
    params->dsi.customization_esd_check_enable = 1;
    params->dsi.lcm_esd_check_table[0].count = 1;
    params->dsi.lcm_esd_check_table[0].para_list[0] = 28;
}

static void lcm_init_power(void)
{
    #ifndef CONFIG_FPGA_EARLY_PORTING
    /* display bias is likely inited in lk !!
     * for kernel regulator system, we need to enable it first before disable!
     * so here, if bias is not enabled, we enable it first
     */
    display_bias_enable();
    #endif
}

static void lcm_suspend_power(void)
{
    #ifndef CONFIG_FPGA_EARLY_PORTING
    display_bias_disable();
    #endif
}

static void lcm_resume_power(void)
{
    #ifndef CONFIG_FPGA_EARLY_PORTING
    SET_RESET_PIN(0);
    display_bias_enable();
    #endif
    
}

static void lcm_init(void)
{
    SET_RESET_PIN(0);
    MDELAY(12);
    SET_RESET_PIN(1);
    MDELAY(12);
    SET_RESET_PIN(0);
    MDELAY(7);
    SET_RESET_PIN(1);
    MDELAY(12);
    
    push_table(NULL, lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
}

static void lcm_suspend(void)
{
    push_table(NULL, lcm_suspend_setting, sizeof(lcm_suspend_setting) / sizeof(struct LCM_setting_table), 1);
    SET_RESET_PIN(0);
    MDELAY(10);
    SET_RESET_PIN(1);
}

static void lcm_resume(void)
{
    lcm_init();
}

static unsigned int lcm_compare_id(void)
{
    return 1;
}

static void lcm_setbacklight_cmdq(void *handle, unsigned int level)
{
    
    LCM_LOGI("%s,td431 backlight: level = %d\n", __func__, level);
    
    bl_level[0].para_list[0] = level;
    
    push_table(handle, bl_level, sizeof(bl_level) / sizeof(struct LCM_setting_table), 1);
}

static void *lcm_switch_mode(int mode)
{
    #ifndef BUILD_LK
    /* customization: 1. V2C config 2 values, C2V config 1 value; 2. config mode control register */
    if (mode == 0) {	/* V2C */
        lcm_switch_mode_cmd.mode = CMD_MODE;
        lcm_switch_mode_cmd.addr = 0xBB;	/* mode control addr */
        lcm_switch_mode_cmd.val[0] = 0x13;	/* enabel GRAM firstly, ensure writing one frame to GRAM */
        lcm_switch_mode_cmd.val[1] = 0x10;	/* disable video mode secondly */
    } else {		/* C2V */
        lcm_switch_mode_cmd.mode = SYNC_PULSE_VDO_MODE;
        lcm_switch_mode_cmd.addr = 0xBB;
        lcm_switch_mode_cmd.val[0] = 0x03;	/* disable GRAM and enable video mode */
    }
    return (void *)(&lcm_switch_mode_cmd);
    #else
    return NULL;
    #endif
}



LCM_DRIVER td4310_fhd_dsi_vdo_chuangwei_malata_lcm_drv = {
    .name = "td4310_fhd_dsi_vdo_chuangwei_malata",
    .set_util_funcs = lcm_set_util_funcs,
    .get_params = lcm_get_params,
    .init = lcm_init,
    .suspend = lcm_suspend,
    .resume = lcm_resume,
    .compare_id = lcm_compare_id,
    .init_power = lcm_init_power,
    .resume_power = lcm_resume_power,
    .suspend_power = lcm_suspend_power,
    .set_backlight_cmdq = lcm_setbacklight_cmdq,
    .switch_mode = lcm_switch_mode,
    
};
