
/*****************************************************************************
*
* Filename:
* ---------
*	 s5k3p3sx_mipi_Sensor.c
*
* Project:
* --------
*	 ALPS
*
* Description:
* ------------
*	 Source code of Sensor driver
*
*
*------------------------------------------------------------------------------
* Reversed by zoggn and adapted for Santin n1
*============================================================================
****************************************************************************/

#include <linux/videodev2.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <asm/atomic.h>
#include <linux/types.h>

#include "kd_camera_typedef.h"
#include "kd_imgsensor.h"
#include "kd_imgsensor_define.h"
#include "kd_imgsensor_errcode.h"

#include "s5k3p3sxmipiraw_Sensor.h"

#define PFX "S5K3P3SX_camera_sensor"
#define LOG_INF(format, args...) pr_debug(PFX "[%s] " format, __FUNCTION__, ##args)

static DEFINE_SPINLOCK(imgsensor_drv_lock);

static imgsensor_info_struct imgsensor_info = {
    .sensor_id = S5K3P3SX_SENSOR_ID,

    .checksum_value = 0xffb1ec31, //0x7b925d1d,

    .pre = {
        .pclk = 280000000, //record different mode's pclk //zoggn
        .linelength = 5148, //record different mode's linelength //zoggn
        .framelength = 1800, //record different mode's framelength //zoggn
        .startx = 0, //record different mode's startx of grabwindow
        .starty = 0, //record different mode's starty of grabwindow
        .grabwindow_width = 2320, //record different mode's width of grabwindow //zoggn
        .grabwindow_height = 1748, //record different mode's height of grabwindow //zoggn
        /*	 following for MIPIDataLowPwr2HighSpeedSettleDelayCount by different scenario	*/
        .mipi_data_lp2hs_settle_dc = 85,
        /*	 following for GetDefaultFramerateByScenario()	*/
        .max_framerate = 300,
    },
    .cap = {
        .pclk = 560000000,
        .linelength = 5148, //zoggn
        .framelength = 1800, //zoggn
        .startx = 0,
        .starty = 0,
        .grabwindow_width = 2320, //zoggn
        .grabwindow_height = 1748, //zoggn
        .mipi_data_lp2hs_settle_dc = 85,
        .max_framerate = 300,
    },
    .cap1 = {
        .pclk = 280000000,
        .linelength = 5148, //zoggn
        .framelength = 1800, //zoggn
        .startx = 0,
        .starty = 0,
        .grabwindow_width = 2320, //zoggn
        .grabwindow_height = 1748, //zoggn
        .mipi_data_lp2hs_settle_dc = 85,
        .max_framerate = 150,
    },
    .cap2 = {
        .pclk = 280000000,
        .linelength = 5148, //zoggn
        .framelength = 1800, //zoggn
        .startx = 0,
        .starty = 0,
        .grabwindow_width = 2320, //zoggn
        .grabwindow_height = 1748, //zoggn
        .mipi_data_lp2hs_settle_dc = 85,
        .max_framerate = 300,
    },
    .normal_video = {
        .pclk = 280000000, //zoggn
        .linelength = 5148, //zoggn
        .framelength = 1800, //zoggn
        .startx = 0,
        .starty = 0,
        .grabwindow_width = 2320, //zoggn
        .grabwindow_height = 1748, //zoggn
        .mipi_data_lp2hs_settle_dc = 85,
        .max_framerate = 300,
    },
    .hs_video = {
        .pclk = 280000000, //zoggn
        .linelength = 5148, //zoggn
        .framelength = 1800, //zoggn
        .startx = 0,
        .starty = 0,
        .grabwindow_width = 1920, //zoggn
        .grabwindow_height = 1080, //zoggn
        .mipi_data_lp2hs_settle_dc = 85,
        .max_framerate = 300,
    },
    .slim_video = {
        .pclk = 280000000, //record different mode's pclk //zoggn
        .linelength = 5148, //record different mode's linelength //zoggn
        .framelength = 1800, //record different mode's framelength //zoggn
        .startx = 0, //record different mode's startx of grabwindow
        .starty = 0, //record different mode's starty of grabwindow
        .grabwindow_width = 2320, //record different mode's width of grabwindow
        .grabwindow_height = 1748, //record different mode's height of grabwindow
        /*	 following for MIPIDataLowPwr2HighSpeedSettleDelayCount by different scenario	*/
        .mipi_data_lp2hs_settle_dc = 85,
        /*	 following for GetDefaultFramerateByScenario()	*/
        .max_framerate = 300,

    },
    .margin = 8, //zoggn
    .min_shutter = 5, //zoggn
    .max_frame_length = 0xffff, //zoggn
    .ae_shut_delay_frame = 0, // zoggn
    .ae_sensor_gain_delay_frame = 0, // zoggn
    .ae_ispGain_delay_frame = 2, // zoggn
    .ihdr_support = 0, //1, support; 0,not support //zoggn
    .ihdr_le_firstline = 0, //1,le first ; 0, se first // zoggn
    .sensor_mode_num = 5, //support sensor mode num // zoggn

    .cap_delay_frame = 3, //zoggn
    .pre_delay_frame = 3, //zoggn
    .video_delay_frame = 3, //zoggn
    .hs_video_delay_frame = 3, //zoggn
    .slim_video_delay_frame = 3, //zoggn

    .isp_driving_current = 2, //zoggn
    .sensor_interface_type = 1, //zoggn
    .mipi_sensor_type = 0, //0,MIPI_OPHY_NCSI2;  1,MIPI_OPHY_CSI2 //zoggn
    .mipi_settle_delay_mode = 1, //0,MIPI_SETTLEDELAY_AUTO; 1,MIPI_SETTLEDELAY_MANNUAL //zoggn
    .sensor_output_dataformat = 2, //zoggn
    .mclk = 24, //zoggn
    .mipi_lane_num = 3, //zoggn
    .i2c_addr_table = { 0x20, 0x5a, 0xff },
};

static imgsensor_struct imgsensor = {
    .mirror = IMAGE_NORMAL, //mirrorflip information
    .sensor_mode = IMGSENSOR_MODE_INIT, //IMGSENSOR_MODE enum value,record current sensor mode,such as: INIT, Preview, Capture, Video,High Speed Video, Slim Video
    .shutter = 0x3D0, //current shutter
    .gain = 0x100, //current gain
    .dummy_pixel = 0, //current dummypixel
    .dummy_line = 0, //current dummyline
    .current_fps = 0, //full size current fps : 24fps for PIP, 30fps for Normal or ZSD
    .autoflicker_en = KAL_FALSE, //auto flicker enable: KAL_FALSE for disable auto flicker, KAL_TRUE for enable auto flicker
    .test_pattern = KAL_FALSE, //test pattern mode or not. KAL_FALSE for in test pattern mode, KAL_TRUE for normal output
    .current_scenario_id = MSDK_SCENARIO_ID_CAMERA_PREVIEW, //current scenario id
    .ihdr_mode = 0, //sensor need support LE, SE with HDR feature
    .i2c_write_id = 0x20,

};

/* Sensor output window information */
static SENSOR_WINSIZE_INFO_STRUCT imgsensor_winsize_info[5] = { { 2320, 1748, 0, 0, 2320, 1748, 2320, 1748, 0, 0, 2320, 1748, 0, 0, 2320, 1748 }, // Preview
    { 2320, 1748, 0, 0, 2320, 1748, 2320, 1748, 0, 0, 2320, 1748, 0, 0, 2320, 1748 }, // capture
    { 2320, 1748, 0, 0, 2320, 1748, 2320, 1748, 0, 0, 2320, 1748, 0, 0, 2320, 1748 }, // video
    { 2320, 1748, 200, 334, 1920, 1080, 1920, 1080, 0, 0, 1920, 1080, 0, 0, 1920, 1080 }, //hight speed video
    { 2320, 1748, 0, 0, 2320, 1748, 2320, 1748, 0, 0, 2320, 1748, 0, 0, 2320, 1748 } }; // slim video

/*//no mirror flip
static SET_PD_BLOCK_INFO_T imgsensor_pd_info =
{
.i4OffsetX = 0,
.i4OffsetY = 4,
.i4PitchX  = 64,
.i4PitchY  = 64,
.i4PairNum  =16,
.i4SubBlkW  =16,
.i4SubBlkH  =16,
.i4PosL = {{8,7},{60,7},{24,11},{44,11},{12,27},{56,27},{28,31},{40,31},{28,39},{40,39},{12,43},{56,43},{24,59},{44,59},{8,63},{60,63}},    
.i4PosR = {{8,11},{60,11},{24,15},{44,15},{12,23},{56,23},{28,27},{40,27},{28,43},{40,43},{12,47},{56,47},{24,55},{44,55},{8,59},{60,59}},
};
*/

extern int iReadReg(u16 a_u2Addr, u8* a_puBuff, u16 i2cId);
extern int iWriteReg(u16 a_u2Addr, u32 a_u4Data, u32 a_u4Bytes, u16 i2cId);
extern void kdSetI2CSpeed(u16 i2cSpeed);
extern bool read_3P3_eeprom(kal_uint16 addr, BYTE* data, kal_uint32 size);

#if 0
static kal_uint16 read_cmos_sensor(kal_uint32 addr)
{
kal_uint16 get_byte=0;
  iReadReg((u16) addr ,(u8*)&get_byte, imgsensor.i2c_write_id);
  return get_byte;
}

#define write_cmos_sensor(addr, para) iWriteReg((u16)addr, (u32)para, 1, imgsensor.i2c_write_id)
#endif
#define RWB_ID_OFFSET 0x0F73
//#define  READ_RWB
#define EEPROM_READ_ID 0xA0
#define EEPROM_WRITE_ID 0xA1

#ifdef READ_RWB
static kal_uint16 is_RWB_sensor(void)
{
    kal_uint16 get_byte = 0;
    char pusendcmd[2] = { (char)(RWB_ID_OFFSET >> 8), (char)(RWB_ID_OFFSET & 0xFF) };
    iReadRegI2C(pusendcmd, 2, (u8*)&get_byte, 1, EEPROM_READ_ID);
    return get_byte;
}
#endif
static kal_uint16 read_cmos_sensor(kal_uint32 addr)
{
    kal_uint16 get_byte = 0;
    char pusendcmd[2] = { (char)(addr >> 8), (char)(addr & 0xFF) };
    iReadRegI2C(pusendcmd, 2, (u8*)&get_byte, 2, imgsensor.i2c_write_id);
    return ((get_byte << 8) & 0xff00) | ((get_byte >> 8) & 0x00ff);
}

static void write_cmos_sensor(kal_uint16 addr, kal_uint16 para)
{
    char pusendcmd[4] = { (char)(addr >> 8), (char)(addr & 0xFF), (char)(para >> 8), (char)(para & 0xFF) };
    iWriteRegI2C(pusendcmd, 4, imgsensor.i2c_write_id);
}

static kal_uint16 read_cmos_sensor_8(kal_uint16 addr)
{
    kal_uint16 get_byte = 0;
    char pusendcmd[2] = { (char)(addr >> 8), (char)(addr & 0xFF) };
    iReadRegI2C(pusendcmd, 2, (u8*)&get_byte, 1, imgsensor.i2c_write_id);
    return get_byte;
}

static void write_cmos_sensor_8(kal_uint16 addr, kal_uint8 para)
{
    char pusendcmd[4] = { (char)(addr >> 8), (char)(addr & 0xFF), (char)(para & 0xFF) };
    iWriteRegI2C(pusendcmd, 3, imgsensor.i2c_write_id);
}

static void set_dummy(void)
{
    LOG_INF("dummyline = %d, dummypixels = %d \n", imgsensor.dummy_line, imgsensor.dummy_pixel);
    //return; //for test
    write_cmos_sensor(0x0340, imgsensor.frame_length);
    write_cmos_sensor(0x0342, imgsensor.line_length);
} /*	set_dummy  */

static void set_max_framerate(UINT16 framerate, kal_bool min_framelength_en)
{

    kal_uint32 frame_length = imgsensor.frame_length;

    LOG_INF("framerate = %d, min framelength should enable %d \n", framerate, min_framelength_en);

    frame_length = imgsensor.pclk / framerate * 10 / imgsensor.line_length;
    spin_lock(&imgsensor_drv_lock);
    if (frame_length >= imgsensor.min_frame_length)
        imgsensor.frame_length = frame_length;
    else
        imgsensor.frame_length = imgsensor.min_frame_length;
    imgsensor.dummy_line = imgsensor.frame_length - imgsensor.min_frame_length;

    if (imgsensor.frame_length > imgsensor_info.max_frame_length) {
        imgsensor.frame_length = imgsensor_info.max_frame_length;
        imgsensor.dummy_line = imgsensor.frame_length - imgsensor.min_frame_length;
    }
    if (min_framelength_en)
        imgsensor.min_frame_length = imgsensor.frame_length;
    spin_unlock(&imgsensor_drv_lock);
    set_dummy();
} /*	set_max_framerate  */

static void write_shutter(kal_uint16 shutter)
{

    kal_uint16 realtime_fps = 0;

    spin_lock(&imgsensor_drv_lock);
    if (shutter > imgsensor.min_frame_length - imgsensor_info.margin)
        imgsensor.frame_length = shutter + imgsensor_info.margin;
    else
        imgsensor.frame_length = imgsensor.min_frame_length;
    if (imgsensor.frame_length > imgsensor_info.max_frame_length)
        imgsensor.frame_length = imgsensor_info.max_frame_length;
    spin_unlock(&imgsensor_drv_lock);
    if (shutter < imgsensor_info.min_shutter)
        shutter = imgsensor_info.min_shutter;

    if (imgsensor.autoflicker_en) {
        realtime_fps = imgsensor.pclk / imgsensor.line_length * 10 / imgsensor.frame_length;
        if (realtime_fps >= 297 && realtime_fps <= 305)
            set_max_framerate(296, 0);
        else if (realtime_fps >= 147 && realtime_fps <= 150)
            set_max_framerate(146, 0);
        else {
            // Extend frame length
            write_cmos_sensor(0x0340, imgsensor.frame_length);
        }
    }
    else {
        // Extend frame length
        write_cmos_sensor(0x0340, imgsensor.frame_length);
    }
    // Update Shutter
    write_cmos_sensor(0x0202, shutter);
    LOG_INF("shutter =%d, framelength =%d\n", shutter, imgsensor.frame_length);

} /*	write_shutter  */

/*************************************************************************
* FUNCTION
*	set_shutter
*
* DESCRIPTION
*	This function set e-shutter of sensor to change exposure time.
*
* PARAMETERS
*	iShutter : exposured lines
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
static void set_shutter(kal_uint16 shutter)
{
    unsigned long flags;
    spin_lock_irqsave(&imgsensor_drv_lock, flags);
    imgsensor.shutter = shutter;
    spin_unlock_irqrestore(&imgsensor_drv_lock, flags);

    write_shutter(shutter);
} /*	set_shutter */

static kal_uint16 gain2reg(const kal_uint16 gain)
{
    kal_uint16 reg_gain = 0x0;

    reg_gain = gain / 2;
    return (kal_uint16)reg_gain;
}

/*************************************************************************
* FUNCTION
*	set_gain
*
* DESCRIPTION
*	This function is to set global gain to sensor.
*
* PARAMETERS
*	iGain : sensor global gain(base: 0x40)
*
* RETURNS
*	the actually gain set to sensor.
*
* GLOBALS AFFECTED
*
*************************************************************************/
static kal_uint16 set_gain(kal_uint16 gain)
{
    kal_uint16 reg_gain;

    //gain=1024;//for test
    //return; //for test

    if (gain < BASEGAIN || gain > 32 * BASEGAIN) {
        LOG_INF("Error gain setting");

        if (gain < BASEGAIN)
            gain = BASEGAIN;
        else if (gain > 32 * BASEGAIN)
            gain = 32 * BASEGAIN;
    }

    reg_gain = gain2reg(gain);
    spin_lock(&imgsensor_drv_lock);
    imgsensor.gain = reg_gain;
    spin_unlock(&imgsensor_drv_lock);
    LOG_INF("gain = %d , reg_gain = 0x%x\n ", gain, reg_gain);

    //write_cmos_sensor(0x0204,reg_gain);
    write_cmos_sensor_8(0x0204, (reg_gain >> 8));
    write_cmos_sensor_8(0x0205, (reg_gain & 0xff));

    return gain;
} /*	set_gain  */

static void set_mirror_flip(kal_uint8 image_mirror)
{

    kal_uint8 itemp;

    LOG_INF("image_mirror = %d\n", image_mirror);
    itemp = read_cmos_sensor(0x0101);
    itemp &= ~0x03;

    switch (image_mirror) {

    case IMAGE_NORMAL:
        write_cmos_sensor(0x0101, itemp);
        break;

    case IMAGE_V_MIRROR:
        write_cmos_sensor(0x0101, itemp | 0x02);
        break;

    case IMAGE_H_MIRROR:
        write_cmos_sensor(0x0101, itemp | 0x01);
        break;

    case IMAGE_HV_MIRROR:
        write_cmos_sensor(0x0101, itemp | 0x03);
        break;
    }
}

/*************************************************************************
* FUNCTION
*	night_mode
*
* DESCRIPTION
*	This function night mode of sensor.
*
* PARAMETERS
*	bEnable: KAL_TRUE -> enable night mode, otherwise, disable night mode
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
#if 0
static void night_mode(kal_bool enable)
{
/*No Need to implement this function*/
}	/*	night_mode	*/
#endif
static void sensor_init(void)
{
    LOG_INF("E\n");

    write_cmos_sensor(0x6028, 0x4000);
    write_cmos_sensor(0x6010, 0x1);
    mdelay(3); // Wait value must be at least 20000 MCLKs
    write_cmos_sensor(0x6028, 0x4000);
    write_cmos_sensor(0x6214, 0x7971);
    write_cmos_sensor(0x6218, 0x100);
    write_cmos_sensor(0x602A, 0xF408);
    write_cmos_sensor(0x6F12, 0x48);
    write_cmos_sensor(0x602A, 0xF40C);
    write_cmos_sensor(0x6F12, 0);
    write_cmos_sensor(0x602A, 0xF4AA);
    write_cmos_sensor(0x6F12, 0x60);
    write_cmos_sensor(0x602A, 0xF442);
    write_cmos_sensor(0x6F12, 0x800);
    write_cmos_sensor(0x602A, 0xF43E);
    write_cmos_sensor(0x6F12, 0x400);
    write_cmos_sensor(0x6F12, 0);
    write_cmos_sensor(0x602A, 0xF4A4);
    write_cmos_sensor(0x6F12, 0x10);
    write_cmos_sensor(0x602A, 0xF4AC);
    write_cmos_sensor(0x6F12, 0x56);
    write_cmos_sensor(0x602A, 0xF480);
    write_cmos_sensor(0x6F12, 8);
    write_cmos_sensor(0x602A, 0xF492);
    write_cmos_sensor(0x6F12, 0x16);
    write_cmos_sensor(0x602A, 0x3E58);
    write_cmos_sensor(0x6F12, 0x56);
    write_cmos_sensor(0x602A, 0x39EE);
    write_cmos_sensor(0x6F12, 0x206);
    write_cmos_sensor(0x602A, 0x39E8);
    write_cmos_sensor(0x6F12, 0x205);
    write_cmos_sensor(0x602A, 0x3A36);
    write_cmos_sensor(0x6F12, 0xB3F0);
    write_cmos_sensor(0x602A, 0x32B2);
    write_cmos_sensor(0x6F12, 0x132);
    write_cmos_sensor(0x602A, 0x3A38);
    write_cmos_sensor(0x6F12, 0x6C);
    write_cmos_sensor(0x602A, 0x3552);
    write_cmos_sensor(0x6F12, 0xD0);
    write_cmos_sensor(0x602A, 0x3194);
    write_cmos_sensor(0x6F12, 0x1001);
    write_cmos_sensor(0x6028, 0x2000);
    write_cmos_sensor(0x602A, 0x13EC);
    write_cmos_sensor(0x6F12, 0x8011);
    write_cmos_sensor(0x6F12, 0x8011);
    write_cmos_sensor(0x6028, 0x4000);
    write_cmos_sensor(0x602A, 0x39BA);
    write_cmos_sensor(0x6F12, 1);
    write_cmos_sensor(0x602A, 0x3004);
    write_cmos_sensor(0x6F12, 8);
    write_cmos_sensor(0x602A, 0x39AA);
    write_cmos_sensor(0x6F12, 0);
    write_cmos_sensor(0x6028, 0x2000);
    write_cmos_sensor(0x602A, 0x26C);
    write_cmos_sensor(0x6F12, 0x41F0);
    write_cmos_sensor(0x6F12, 0);
    write_cmos_sensor(0x6028, 0x4000);
    write_cmos_sensor(0x602A, 0x37D4);
    write_cmos_sensor(0x6F12, 0x2D);
    write_cmos_sensor(0x602A, 0x37DA);
    write_cmos_sensor(0x6F12, 0x5D);
    write_cmos_sensor(0x602A, 0x37E0);
    write_cmos_sensor(0x6F12, 0x8D);
    write_cmos_sensor(0x602A, 0x37E6);
    write_cmos_sensor(0x6F12, 0xBD);
    write_cmos_sensor(0x602A, 0x37EC);
    write_cmos_sensor(0x6F12, 0xED);
    write_cmos_sensor(0x602A, 0x37F2);
    write_cmos_sensor(0x6F12, 0x11D);
    write_cmos_sensor(0x602A, 0x37F8);
    write_cmos_sensor(0x6F12, 0x14D);
    write_cmos_sensor(0x602A, 0x37FE);
    write_cmos_sensor(0x6F12, 0x17D);
    write_cmos_sensor(0x602A, 0x3804);
    write_cmos_sensor(0x6F12, 0x1AD);
    write_cmos_sensor(0x602A, 0x380A);
    write_cmos_sensor(0x6F12, 0x1DD);
    write_cmos_sensor(0x602A, 0x3810);
    write_cmos_sensor(0x6F12, 0x20D);
    write_cmos_sensor(0x602A, 0x32A6);
    write_cmos_sensor(0x6F12, 6);
    write_cmos_sensor(0x602A, 0x32BE);
    write_cmos_sensor(0x6F12, 6);
    write_cmos_sensor(0x602A, 0x3210);
    write_cmos_sensor(0x6F12, 6);
    write_cmos_sensor(0x6028, 8192);
    write_cmos_sensor(0x602A, 0x2EF8);
    write_cmos_sensor(0x6F12, 0);
    write_cmos_sensor(0x6F12, 0);
    write_cmos_sensor(0x6F12, 0);
    write_cmos_sensor(0x6F12, 0);
    write_cmos_sensor(0x6F12, 0x448);
    write_cmos_sensor(0x6F12, 0x349);
    write_cmos_sensor(0x6F12, 0x160);
    write_cmos_sensor(0x6F12, 0xC26A);
    write_cmos_sensor(0x6F12, 0x511A);
    write_cmos_sensor(0x6F12, 0x8180);
    write_cmos_sensor(0x6F12, 0xF0);
    write_cmos_sensor(0x6F12, 0x21B8);
    write_cmos_sensor(0x6F12, 0x2000);
    write_cmos_sensor(0x6F12, 0x2F78);
    write_cmos_sensor(0x6F12, 0x2000);
    write_cmos_sensor(0x6F12, 0x18E0);
    write_cmos_sensor(0x6F12, 0);
    write_cmos_sensor(0x6F12, 0);
    write_cmos_sensor(0x6F12, 0);
    write_cmos_sensor(0x6F12, 0);
    write_cmos_sensor(0x6F12, 0x10B5);
    write_cmos_sensor(0x6F12, 0xE4C);
    write_cmos_sensor(0x6F12, 0xB4F8);
    write_cmos_sensor(0x6F12, 0x5820);
    write_cmos_sensor(0x6F12, 0x2388);
    write_cmos_sensor(0x6F12, 0xA2EB);
    write_cmos_sensor(0x6F12, 0x5302);
    write_cmos_sensor(0x6F12, 0xA4F8);
    write_cmos_sensor(0x6F12, 0x5820);
    write_cmos_sensor(0x6F12, 0x6288);
    write_cmos_sensor(0x6F12, 0x5208);
    write_cmos_sensor(0x6F12, 0x6280);
    write_cmos_sensor(0x6F12, 0xF0);
    write_cmos_sensor(0x6F12, 0x14F8);
    write_cmos_sensor(0x6F12, 0xB4F8);
    write_cmos_sensor(0x6F12, 0x5800);
    write_cmos_sensor(0x6F12, 0x2188);
    write_cmos_sensor(0x6F12, 0xEB);
    write_cmos_sensor(0x6F12, 0x5100);
    write_cmos_sensor(0x6F12, 0xA4F8);
    write_cmos_sensor(0x6F12, 0x5800);
    write_cmos_sensor(0x6F12, 0x6088);
    write_cmos_sensor(0x6F12, 0x4000);
    write_cmos_sensor(0x6F12, 0x6080);
    write_cmos_sensor(0x6F12, 0x10BD);
    write_cmos_sensor(0x6F12, 0xAFF2);
    write_cmos_sensor(0x6F12, 0x3300);
    write_cmos_sensor(0x6F12, 0x249);
    write_cmos_sensor(0x6F12, 0x863);
    write_cmos_sensor(0x6F12, 0x7047);
    write_cmos_sensor(0x6F12, 0x2000);
    write_cmos_sensor(0x6F12, 0x1998);
    write_cmos_sensor(0x6F12, 0x2000);
    write_cmos_sensor(0x6F12, 0x460);
    write_cmos_sensor(0x6F12, 0x43F2);
    write_cmos_sensor(0x6F12, 0x290C);
    write_cmos_sensor(0x6F12, 0xC0F2);
    write_cmos_sensor(0x6F12, 0xC);
    write_cmos_sensor(0x6F12, 0x6047);
    write_cmos_sensor(0x6F12, 0);
    write_cmos_sensor(0x6F12, 0x3103);
    write_cmos_sensor(0x6F12, 0x22);
    write_cmos_sensor(0x6F12, 0);
    write_cmos_sensor(0x6F12, 1);

} /*	sensor_init  */

static void preview_setting(void)
{
    //Preview 2104*1560 30fps 24M MCLK 4lane 608Mbps/lane
    // preview 30.01fps
    LOG_INF("E\n");
    /* stream off */
    write_cmos_sensor(0x100, 0);

    write_cmos_sensor(0x6028, 0x4000);
    write_cmos_sensor(0x344, 0);
    write_cmos_sensor(0x346, 0);
    write_cmos_sensor(0x348, 0x90F);
    write_cmos_sensor(0x34A, 0x6D3);
    write_cmos_sensor(0x34C, 0x910);
    write_cmos_sensor(0x34E, 0x6D4);
    write_cmos_sensor(0x3002, 1);
    write_cmos_sensor(0x136, 0x1800);
    write_cmos_sensor(0x304, 6);
    write_cmos_sensor(0x306, 0x8C);
    write_cmos_sensor(0x302, 1);
    write_cmos_sensor(0x300, 8);
    write_cmos_sensor(0x30C, 4);
    write_cmos_sensor(0x30E, 0x78);
    write_cmos_sensor(0x30A, 1);
    write_cmos_sensor(0x308, 8);
    write_cmos_sensor(0x3008, 1);
    write_cmos_sensor(0x800, 0);
    write_cmos_sensor(0x200, 0x200);
    write_cmos_sensor(0x202, 0x100);
    write_cmos_sensor(0x21C, 0x200);
    write_cmos_sensor(0x21E, 0x100);
    write_cmos_sensor(0x342, 0x141C);
    write_cmos_sensor(0x340, 0x708);
    write_cmos_sensor(0x3072, 0x3C0);
    write_cmos_sensor(0x6214, 0x7970);

    /* stream on */
    write_cmos_sensor(0x100, 0x100);
} /*	preview_setting  */

static void capture_setting(kal_uint16 currefps)
{
    LOG_INF("E! currefps:%d\n", currefps);
    // full size 29.76fps
    // capture setting 4208*3120  480MCLK 1.2Gp/lane

    /* stream off */
    write_cmos_sensor(0x100, 0);

    write_cmos_sensor(0x6028, 0x4000);
    write_cmos_sensor(0x344, 0);
    write_cmos_sensor(0x346, 0);
    write_cmos_sensor(0x348, 0x90F);
    write_cmos_sensor(0x34A, 0x6D3);
    write_cmos_sensor(0x34C, 0x910);
    write_cmos_sensor(0x34E, 0x6D4);
    write_cmos_sensor(0x3002, 1);
    write_cmos_sensor(0x136, 0x1800);
    write_cmos_sensor(0x304, 6);
    write_cmos_sensor(0x306, 0x8C);
    write_cmos_sensor(0x302, 1);
    write_cmos_sensor(0x300, 8);
    write_cmos_sensor(0x30C, 4);
    write_cmos_sensor(0x30E, 0x78);
    write_cmos_sensor(0x30A, 1);
    write_cmos_sensor(0x308, 8);
    write_cmos_sensor(0x3008, 1);
    write_cmos_sensor(0x800, 0);
    write_cmos_sensor(0x200, 0x200);
    write_cmos_sensor(0x202, 0x100);
    write_cmos_sensor(0x21C, 0x200);
    write_cmos_sensor(0x21E, 0x100);
    write_cmos_sensor(0x342, 0x141C);
    write_cmos_sensor(0x340, 0x708);
    write_cmos_sensor(0x3072, 0x3C0);
    write_cmos_sensor(0x6214, 0x7970);

    /* stream on */
    write_cmos_sensor(0x100, 0x100);
}
static void normal_video_setting(kal_uint16 currefps)
{
    LOG_INF("E! currefps:%d\n", currefps);
    // full size 30fps
    capture_setting(300);
}
static void hs_video_setting(void)
{
    LOG_INF("E\n");
    /* stream off */
    write_cmos_sensor(0x100, 0);

    write_cmos_sensor(0x6028, 0x4000);
    write_cmos_sensor(0x344, 0xC8);
    write_cmos_sensor(0x346, 0x14E);
    write_cmos_sensor(0x348, 0x847);
    write_cmos_sensor(0x34A, 0x585);
    write_cmos_sensor(0x34C, 0x780);
    write_cmos_sensor(0x34E, 0x438);
    write_cmos_sensor(0x3002, 1);
    write_cmos_sensor(0x136, 0x1800);
    write_cmos_sensor(0x304, 6);
    write_cmos_sensor(0x306, 0x8C);
    write_cmos_sensor(0x302, 1);
    write_cmos_sensor(0x300, 8);
    write_cmos_sensor(0x30C, 4);
    write_cmos_sensor(0x30E, 0x78);
    write_cmos_sensor(0x30A, 1);
    write_cmos_sensor(0x308, 8);
    write_cmos_sensor(0x3008, 1);
    write_cmos_sensor(0x800, 0);
    write_cmos_sensor(0x200, 0x200);
    write_cmos_sensor(0x202, 0x100);
    write_cmos_sensor(0x21C, 0x200);
    write_cmos_sensor(0x21E, 0x100);
    write_cmos_sensor(0x342, 0x141C);
    write_cmos_sensor(0x340, 0x708);
    write_cmos_sensor(0x3072, 0x3C0);
    write_cmos_sensor(0x6214, 0x7970);

    /* stream on */
    write_cmos_sensor(0x100, 0x100);
}

static void slim_video_setting(void)
{
    LOG_INF("E\n");
    preview_setting();
}

/*************************************************************************
* FUNCTION
*	get_imgsensor_id
*
* DESCRIPTION
*	This function get the sensor ID
*
* PARAMETERS
*	*sensorID : return the sensor ID
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
static kal_uint32 get_imgsensor_id(UINT32* sensor_id)
{
    kal_uint8 i = 0;
    kal_uint8 retry = 2;
    //sensor have two i2c address 0x6c 0x6d & 0x21 0x20, we should detect the module used i2c address
    while (imgsensor_info.i2c_addr_table[i] != 0xff) {
        spin_lock(&imgsensor_drv_lock);
        imgsensor.i2c_write_id = imgsensor_info.i2c_addr_table[i];
        spin_unlock(&imgsensor_drv_lock);
        do {
            *sensor_id = ((read_cmos_sensor_8(0x0000) << 8) | read_cmos_sensor_8(0x0001));
            LOG_INF("read_0x0000=0x%x, 0x0001=0x%x,0x0000_0001=0x%x\n", read_cmos_sensor_8(0x0000), read_cmos_sensor_8(0x0001), read_cmos_sensor(0x0000));
            if (*sensor_id == imgsensor_info.sensor_id) {
                LOG_INF("i2c write id: 0x%x, sensor id: 0x%x\n", imgsensor.i2c_write_id, *sensor_id);
#ifdef READ_RWB // added by zoggn
                if (is_RWB_sensor() == 0x1) {
                    imgsensor_info.sensor_output_dataformat = SENSOR_OUTPUT_FORMAT_RAW_RWB_Wr;
                    LOG_INF("RWB sensor of S5k3p3\n");
                }
#endif
                return ERROR_NONE;
            }
            LOG_INF("Read sensor id fail, id: 0x%x\n", imgsensor.i2c_write_id);
            retry--;
        } while (retry > 0);
        i++;
        retry = 2;
    }
    if (*sensor_id != imgsensor_info.sensor_id) {
        // if Sensor ID is not correct, Must set *sensor_id to 0xFFFFFFFF
        *sensor_id = 0xFFFFFFFF;
        return ERROR_SENSOR_CONNECT_FAIL;
    }
    return ERROR_NONE;
}

/*************************************************************************
* FUNCTION
*	open
*
* DESCRIPTION
*	This function initialize the registers of CMOS sensor
*
* PARAMETERS
*	None
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
static kal_uint32 open(void)
{
    kal_uint8 i = 0;
    kal_uint8 retry = 2;
    kal_uint16 sensor_id = 0;
    LOG_INF("PLATFORM:MT6595,MIPI 2LANE\n");
    LOG_INF("preview 1280*960@30fps,864Mbps/lane; video 1280*960@30fps,864Mbps/lane; capture 5M@30fps,864Mbps/lane\n");

    //sensor have two i2c address 0x6c 0x6d & 0x21 0x20, we should detect the module used i2c address
    while (imgsensor_info.i2c_addr_table[i] != 0xff) {
        spin_lock(&imgsensor_drv_lock);
        imgsensor.i2c_write_id = imgsensor_info.i2c_addr_table[i];
        spin_unlock(&imgsensor_drv_lock);
        do {
            sensor_id = ((read_cmos_sensor_8(0x0000) << 8) | read_cmos_sensor_8(0x0001));
            if (sensor_id == imgsensor_info.sensor_id) {
                LOG_INF("i2c write id: 0x%x, sensor id: 0x%x\n", imgsensor.i2c_write_id, sensor_id);
                break;
            }
            LOG_INF("Read sensor id fail, id: 0x%x\n", imgsensor.i2c_write_id);
            retry--;
        } while (retry > 0);
        i++;
        if (sensor_id == imgsensor_info.sensor_id)
            break;
        retry = 2;
    }
    if (imgsensor_info.sensor_id != sensor_id)
        return ERROR_SENSOR_CONNECT_FAIL;

    /* initail sequence write in  */
    sensor_init();

    spin_lock(&imgsensor_drv_lock);

    imgsensor.autoflicker_en = KAL_FALSE;
    imgsensor.sensor_mode = IMGSENSOR_MODE_INIT;
    imgsensor.shutter = 0x3D0;
    imgsensor.gain = 0x100;
    imgsensor.pclk = imgsensor_info.pre.pclk;
    imgsensor.frame_length = imgsensor_info.pre.framelength;
    imgsensor.line_length = imgsensor_info.pre.linelength;
    imgsensor.min_frame_length = imgsensor_info.pre.framelength;
    imgsensor.dummy_pixel = 0;
    imgsensor.dummy_line = 0;
    imgsensor.ihdr_mode = 0;
    imgsensor.test_pattern = KAL_FALSE;
    imgsensor.current_fps = imgsensor_info.pre.max_framerate;
    spin_unlock(&imgsensor_drv_lock);

    return ERROR_NONE;
} /*	open  */

/*************************************************************************
* FUNCTION
*	close
*
* DESCRIPTION
*
*
* PARAMETERS
*	None
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
static kal_uint32 close(void)
{
    LOG_INF("E\n");

    /*No Need to implement this function*/

    return ERROR_NONE;
} /*	close  */

/*************************************************************************
* FUNCTION
* preview
*
* DESCRIPTION
*	This function start the sensor preview.
*
* PARAMETERS
*	*image_window : address pointer of pixel numbers in one period of HSYNC
*  *sensor_config_data : address pointer of line numbers in one period of VSYNC
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
static kal_uint32 preview(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT* image_window,
    MSDK_SENSOR_CONFIG_STRUCT* sensor_config_data)
{
    LOG_INF("E\n");

    spin_lock(&imgsensor_drv_lock);
    imgsensor.sensor_mode = IMGSENSOR_MODE_PREVIEW;
    imgsensor.pclk = imgsensor_info.pre.pclk;
    imgsensor.line_length = imgsensor_info.pre.linelength;
    imgsensor.frame_length = imgsensor_info.pre.framelength;
    imgsensor.min_frame_length = imgsensor_info.pre.framelength;
    imgsensor.autoflicker_en = KAL_FALSE;
    spin_unlock(&imgsensor_drv_lock);

    preview_setting();
    set_mirror_flip(imgsensor.mirror);

    return ERROR_NONE;
} /*	preview   */

/*************************************************************************
* FUNCTION
*	capture
*
* DESCRIPTION
*	This function setup the CMOS sensor in capture MY_OUTPUT mode
*
* PARAMETERS
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
static kal_uint32 capture(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT* image_window,
    MSDK_SENSOR_CONFIG_STRUCT* sensor_config_data)
{
    LOG_INF("E\n");
    spin_lock(&imgsensor_drv_lock);
    imgsensor.sensor_mode = IMGSENSOR_MODE_CAPTURE;
    if (imgsensor.current_fps == imgsensor_info.cap1.max_framerate) { //PIP capture: 24fps for less than 13M, 20fps for 16M,15fps for 20M
        imgsensor.pclk = imgsensor_info.cap1.pclk;
        imgsensor.line_length = imgsensor_info.cap1.linelength;
        imgsensor.frame_length = imgsensor_info.cap1.framelength;
        imgsensor.min_frame_length = imgsensor_info.cap1.framelength;
        imgsensor.autoflicker_en = KAL_FALSE;
    }
    else if (imgsensor.current_fps == imgsensor_info.cap2.max_framerate) {
        imgsensor.pclk = imgsensor_info.cap2.pclk;
        imgsensor.line_length = imgsensor_info.cap2.linelength;
        imgsensor.frame_length = imgsensor_info.cap2.framelength;
        imgsensor.min_frame_length = imgsensor_info.cap2.framelength;
        imgsensor.autoflicker_en = KAL_FALSE;
    }
    else {
        if (imgsensor.current_fps != imgsensor_info.cap.max_framerate)
            LOG_INF("Warning: current_fps %d fps is not support, so use cap's setting: %d fps!\n", imgsensor.current_fps, imgsensor_info.cap.max_framerate / 10);
        imgsensor.pclk = imgsensor_info.cap.pclk;
        imgsensor.line_length = imgsensor_info.cap.linelength;
        imgsensor.frame_length = imgsensor_info.cap.framelength;
        imgsensor.min_frame_length = imgsensor_info.cap.framelength;
        imgsensor.autoflicker_en = KAL_FALSE;
    }
    spin_unlock(&imgsensor_drv_lock);

    capture_setting(imgsensor.current_fps);
    set_mirror_flip(imgsensor.mirror);

    return ERROR_NONE;
} /* capture() */
static kal_uint32 normal_video(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT* image_window,
    MSDK_SENSOR_CONFIG_STRUCT* sensor_config_data)
{
    LOG_INF("E\n");

    spin_lock(&imgsensor_drv_lock);
    imgsensor.sensor_mode = IMGSENSOR_MODE_VIDEO;
    imgsensor.pclk = imgsensor_info.normal_video.pclk;
    imgsensor.line_length = imgsensor_info.normal_video.linelength;
    imgsensor.frame_length = imgsensor_info.normal_video.framelength;
    imgsensor.min_frame_length = imgsensor_info.normal_video.framelength;
    imgsensor.autoflicker_en = KAL_FALSE;
    spin_unlock(&imgsensor_drv_lock);

    normal_video_setting(imgsensor.current_fps);
    set_mirror_flip(imgsensor.mirror);

    return ERROR_NONE;
} /*	normal_video   */

static kal_uint32 hs_video(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT* image_window,
    MSDK_SENSOR_CONFIG_STRUCT* sensor_config_data)
{
    LOG_INF("E\n");

    spin_lock(&imgsensor_drv_lock);
    imgsensor.sensor_mode = IMGSENSOR_MODE_HIGH_SPEED_VIDEO;
    imgsensor.pclk = imgsensor_info.hs_video.pclk;
    //imgsensor.video_mode = KAL_TRUE;
    imgsensor.line_length = imgsensor_info.hs_video.linelength;
    imgsensor.frame_length = imgsensor_info.hs_video.framelength;
    imgsensor.min_frame_length = imgsensor_info.hs_video.framelength;
    imgsensor.dummy_line = 0;
    imgsensor.dummy_pixel = 0;
    //imgsensor.current_fps = 300;
    imgsensor.autoflicker_en = KAL_FALSE;
    spin_unlock(&imgsensor_drv_lock);
    hs_video_setting();
    set_mirror_flip(imgsensor.mirror);

    return ERROR_NONE;
} /*	hs_video   */

static kal_uint32 slim_video(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT* image_window,
    MSDK_SENSOR_CONFIG_STRUCT* sensor_config_data)
{
    LOG_INF("E\n");

    spin_lock(&imgsensor_drv_lock);
    imgsensor.sensor_mode = IMGSENSOR_MODE_SLIM_VIDEO;
    imgsensor.pclk = imgsensor_info.slim_video.pclk;
    //imgsensor.video_mode = KAL_TRUE;
    imgsensor.line_length = imgsensor_info.slim_video.linelength;
    imgsensor.frame_length = imgsensor_info.slim_video.framelength;
    imgsensor.min_frame_length = imgsensor_info.slim_video.framelength;
    imgsensor.dummy_line = 0;
    imgsensor.dummy_pixel = 0;
    //imgsensor.current_fps = 300;
    imgsensor.autoflicker_en = KAL_FALSE;
    spin_unlock(&imgsensor_drv_lock);
    slim_video_setting();
    set_mirror_flip(imgsensor.mirror);

    return ERROR_NONE;
} /*	slim_video	 */

static kal_uint32 get_resolution(MSDK_SENSOR_RESOLUTION_INFO_STRUCT* sensor_resolution)
{
    LOG_INF("E\n");
    sensor_resolution->SensorFullWidth = imgsensor_info.cap.grabwindow_width;
    sensor_resolution->SensorFullHeight = imgsensor_info.cap.grabwindow_height;

    sensor_resolution->SensorPreviewWidth = imgsensor_info.pre.grabwindow_width;
    sensor_resolution->SensorPreviewHeight = imgsensor_info.pre.grabwindow_height;

    sensor_resolution->SensorVideoWidth = imgsensor_info.normal_video.grabwindow_width;
    sensor_resolution->SensorVideoHeight = imgsensor_info.normal_video.grabwindow_height;

    sensor_resolution->SensorHighSpeedVideoWidth = imgsensor_info.hs_video.grabwindow_width;
    sensor_resolution->SensorHighSpeedVideoHeight = imgsensor_info.hs_video.grabwindow_height;

    sensor_resolution->SensorSlimVideoWidth = imgsensor_info.slim_video.grabwindow_width;
    sensor_resolution->SensorSlimVideoHeight = imgsensor_info.slim_video.grabwindow_height;
    return ERROR_NONE;
} /*	get_resolution	*/

static kal_uint32 get_info(MSDK_SCENARIO_ID_ENUM scenario_id,
    MSDK_SENSOR_INFO_STRUCT* sensor_info,
    MSDK_SENSOR_CONFIG_STRUCT* sensor_config_data)
{
    LOG_INF("scenario_id = %d\n", scenario_id);

    sensor_info->SensorClockPolarity = SENSOR_CLOCK_POLARITY_LOW;
    sensor_info->SensorClockFallingPolarity = SENSOR_CLOCK_POLARITY_LOW; /* not use */
    sensor_info->SensorHsyncPolarity = SENSOR_CLOCK_POLARITY_LOW; // inverse with datasheet
    sensor_info->SensorVsyncPolarity = SENSOR_CLOCK_POLARITY_LOW;
    sensor_info->SensorInterruptDelayLines = 4; /* not use */
    sensor_info->SensorResetActiveHigh = FALSE; /* not use */
    sensor_info->SensorResetDelayCount = 5; /* not use */

    sensor_info->SensroInterfaceType = imgsensor_info.sensor_interface_type;
    sensor_info->MIPIsensorType = imgsensor_info.mipi_sensor_type;
    sensor_info->SettleDelayMode = imgsensor_info.mipi_settle_delay_mode;
    sensor_info->SensorOutputDataFormat = imgsensor_info.sensor_output_dataformat;

    sensor_info->CaptureDelayFrame = imgsensor_info.cap_delay_frame;
    sensor_info->PreviewDelayFrame = imgsensor_info.pre_delay_frame;
    sensor_info->VideoDelayFrame = imgsensor_info.video_delay_frame;
    sensor_info->HighSpeedVideoDelayFrame = imgsensor_info.hs_video_delay_frame;
    sensor_info->SlimVideoDelayFrame = imgsensor_info.slim_video_delay_frame;

    sensor_info->SensorMasterClockSwitch = 0; /* not use */
    sensor_info->SensorDrivingCurrent = imgsensor_info.isp_driving_current;

    sensor_info->AEShutDelayFrame = imgsensor_info.ae_shut_delay_frame; /* The frame of setting shutter default 0 for TG int */
    sensor_info->AESensorGainDelayFrame = imgsensor_info.ae_sensor_gain_delay_frame; /* The frame of setting sensor gain */
    sensor_info->AEISPGainDelayFrame = imgsensor_info.ae_ispGain_delay_frame;
    sensor_info->IHDR_Support = imgsensor_info.ihdr_support;
    sensor_info->IHDR_LE_FirstLine = imgsensor_info.ihdr_le_firstline;
    sensor_info->SensorModeNum = imgsensor_info.sensor_mode_num;
    sensor_info->PDAF_Support = 0;
    sensor_info->SensorMIPILaneNumber = imgsensor_info.mipi_lane_num;
    sensor_info->SensorClockFreq = imgsensor_info.mclk;
    sensor_info->SensorClockDividCount = 3; /* not use */
    sensor_info->SensorClockRisingCount = 0;
    sensor_info->SensorClockFallingCount = 2; /* not use */
    sensor_info->SensorPixelClockCount = 3; /* not use */
    sensor_info->SensorDataLatchCount = 2; /* not use */

    sensor_info->MIPIDataLowPwr2HighSpeedTermDelayCount = 0;
    sensor_info->MIPICLKLowPwr2HighSpeedTermDelayCount = 0;
    sensor_info->SensorWidthSampling = 0; // 0 is default 1x
    sensor_info->SensorHightSampling = 0; // 0 is default 1x
    sensor_info->SensorPacketECCOrder = 1;

    switch (scenario_id) {
    case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
        sensor_info->SensorGrabStartX = imgsensor_info.pre.startx;
        sensor_info->SensorGrabStartY = imgsensor_info.pre.starty;

        sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount = imgsensor_info.pre.mipi_data_lp2hs_settle_dc;

        break;
    case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
        sensor_info->SensorGrabStartX = imgsensor_info.cap.startx;
        sensor_info->SensorGrabStartY = imgsensor_info.cap.starty;

        sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount = imgsensor_info.cap.mipi_data_lp2hs_settle_dc;

        break;
    case MSDK_SCENARIO_ID_VIDEO_PREVIEW:

        sensor_info->SensorGrabStartX = imgsensor_info.normal_video.startx;
        sensor_info->SensorGrabStartY = imgsensor_info.normal_video.starty;

        sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount = imgsensor_info.normal_video.mipi_data_lp2hs_settle_dc;

        break;
    case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
        sensor_info->SensorGrabStartX = imgsensor_info.hs_video.startx;
        sensor_info->SensorGrabStartY = imgsensor_info.hs_video.starty;

        sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount = imgsensor_info.hs_video.mipi_data_lp2hs_settle_dc;

        break;
    case MSDK_SCENARIO_ID_SLIM_VIDEO:
        sensor_info->SensorGrabStartX = imgsensor_info.slim_video.startx;
        sensor_info->SensorGrabStartY = imgsensor_info.slim_video.starty;

        sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount = imgsensor_info.slim_video.mipi_data_lp2hs_settle_dc;

        break;
    default:
        sensor_info->SensorGrabStartX = imgsensor_info.pre.startx;
        sensor_info->SensorGrabStartY = imgsensor_info.pre.starty;

        sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount = imgsensor_info.pre.mipi_data_lp2hs_settle_dc;
        break;
    }

    return ERROR_NONE;
} /*	get_info  */

static kal_uint32 control(MSDK_SCENARIO_ID_ENUM scenario_id, MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT* image_window,
    MSDK_SENSOR_CONFIG_STRUCT* sensor_config_data)
{
    LOG_INF("scenario_id = %d\n", scenario_id);
    spin_lock(&imgsensor_drv_lock);
    imgsensor.current_scenario_id = scenario_id;
    spin_unlock(&imgsensor_drv_lock);
    switch (scenario_id) {
    case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
        preview(image_window, sensor_config_data);
        break;
    case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
        capture(image_window, sensor_config_data);
        break;
    case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
        normal_video(image_window, sensor_config_data);
        break;
    case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
        hs_video(image_window, sensor_config_data);
        break;
    case MSDK_SCENARIO_ID_SLIM_VIDEO:
        slim_video(image_window, sensor_config_data);
        break;
    default:
        LOG_INF("Error ScenarioId setting");
        preview(image_window, sensor_config_data);
        return ERROR_INVALID_SCENARIO_ID;
    }
    return ERROR_NONE;
} /* control() */

static kal_uint32 set_video_mode(UINT16 framerate)
{
    LOG_INF("framerate = %d\n ", framerate);
    // SetVideoMode Function should fix framerate
    if (framerate == 0)
        // Dynamic frame rate
        return ERROR_NONE;
    spin_lock(&imgsensor_drv_lock);
    if ((framerate == 300) && (imgsensor.autoflicker_en == KAL_TRUE))
        imgsensor.current_fps = 296;
    else if ((framerate == 150) && (imgsensor.autoflicker_en == KAL_TRUE))
        imgsensor.current_fps = 146;
    else
        imgsensor.current_fps = framerate;
    spin_unlock(&imgsensor_drv_lock);
    set_max_framerate(imgsensor.current_fps, 1);

    return ERROR_NONE;
}

static kal_uint32 set_auto_flicker_mode(kal_bool enable, UINT16 framerate)
{
    LOG_INF("enable = %d, framerate = %d \n", enable, framerate);
    spin_lock(&imgsensor_drv_lock);
    if (enable) //enable auto flicker
        imgsensor.autoflicker_en = KAL_TRUE;
    else //Cancel Auto flick
        imgsensor.autoflicker_en = KAL_FALSE;
    spin_unlock(&imgsensor_drv_lock);
    return ERROR_NONE;
}

static kal_uint32 set_max_framerate_by_scenario(MSDK_SCENARIO_ID_ENUM scenario_id, MUINT32 framerate)
{
    kal_uint32 frame_length;

    LOG_INF("scenario_id = %d, framerate = %d\n", scenario_id, framerate);

    switch (scenario_id) {
    case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
        frame_length = imgsensor_info.pre.pclk / framerate * 10 / imgsensor_info.pre.linelength;
        spin_lock(&imgsensor_drv_lock);
        imgsensor.dummy_line = (frame_length > imgsensor_info.pre.framelength) ? (frame_length - imgsensor_info.pre.framelength) : 0;
        imgsensor.frame_length = imgsensor_info.pre.framelength + imgsensor.dummy_line;
        imgsensor.min_frame_length = imgsensor.frame_length;
        spin_unlock(&imgsensor_drv_lock);
        //set_dummy();
        break;
    case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
        if (framerate == 0)
            return ERROR_NONE;
        frame_length = imgsensor_info.normal_video.pclk / framerate * 10 / imgsensor_info.normal_video.linelength;
        spin_lock(&imgsensor_drv_lock);
        imgsensor.dummy_line = (frame_length > imgsensor_info.normal_video.framelength) ? (frame_length - imgsensor_info.normal_video.framelength) : 0;
        imgsensor.frame_length = imgsensor_info.normal_video.framelength + imgsensor.dummy_line;
        imgsensor.min_frame_length = imgsensor.frame_length;
        spin_unlock(&imgsensor_drv_lock);
        //set_dummy();
        break;
    case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
        if (imgsensor.current_fps == imgsensor_info.cap1.max_framerate) {
            frame_length = imgsensor_info.cap1.pclk / framerate * 10 / imgsensor_info.cap1.linelength;
            spin_lock(&imgsensor_drv_lock);
            imgsensor.dummy_line = (frame_length > imgsensor_info.cap1.framelength) ? (frame_length - imgsensor_info.cap1.framelength) : 0;
            imgsensor.frame_length = imgsensor_info.cap1.framelength + imgsensor.dummy_line;
            imgsensor.min_frame_length = imgsensor.frame_length;
            spin_unlock(&imgsensor_drv_lock);
        }
        else if (imgsensor.current_fps == imgsensor_info.cap2.max_framerate) {
            frame_length = imgsensor_info.cap2.pclk / framerate * 10 / imgsensor_info.cap2.linelength;
            spin_lock(&imgsensor_drv_lock);
            imgsensor.dummy_line = (frame_length > imgsensor_info.cap2.framelength) ? (frame_length - imgsensor_info.cap2.framelength) : 0;
            imgsensor.frame_length = imgsensor_info.cap2.framelength + imgsensor.dummy_line;
            imgsensor.min_frame_length = imgsensor.frame_length;
            spin_unlock(&imgsensor_drv_lock);
        }
        else {
            if (imgsensor.current_fps != imgsensor_info.cap.max_framerate)
                LOG_INF("Warning: current_fps %d fps is not support, so use cap's setting: %d fps!\n", framerate, imgsensor_info.cap.max_framerate / 10);
            frame_length = imgsensor_info.cap.pclk / framerate * 10 / imgsensor_info.cap.linelength;
            spin_lock(&imgsensor_drv_lock);
            imgsensor.dummy_line = (frame_length > imgsensor_info.cap.framelength) ? (frame_length - imgsensor_info.cap.framelength) : 0;
            imgsensor.frame_length = imgsensor_info.cap.framelength + imgsensor.dummy_line;
            imgsensor.min_frame_length = imgsensor.frame_length;
            spin_unlock(&imgsensor_drv_lock);
        }
        //set_dummy();
        break;
    case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
        frame_length = imgsensor_info.hs_video.pclk / framerate * 10 / imgsensor_info.hs_video.linelength;
        spin_lock(&imgsensor_drv_lock);
        imgsensor.dummy_line = (frame_length > imgsensor_info.hs_video.framelength) ? (frame_length - imgsensor_info.hs_video.framelength) : 0;
        imgsensor.frame_length = imgsensor_info.hs_video.framelength + imgsensor.dummy_line;
        imgsensor.min_frame_length = imgsensor.frame_length;
        spin_unlock(&imgsensor_drv_lock);
        //set_dummy();
        break;
    case MSDK_SCENARIO_ID_SLIM_VIDEO:
        frame_length = imgsensor_info.slim_video.pclk / framerate * 10 / imgsensor_info.slim_video.linelength;
        spin_lock(&imgsensor_drv_lock);
        imgsensor.dummy_line = (frame_length > imgsensor_info.slim_video.framelength) ? (frame_length - imgsensor_info.slim_video.framelength) : 0;
        imgsensor.frame_length = imgsensor_info.slim_video.framelength + imgsensor.dummy_line;
        imgsensor.min_frame_length = imgsensor.frame_length;
        spin_unlock(&imgsensor_drv_lock);
        //set_dummy();
        break;
    default: //coding with  preview scenario by default
        frame_length = imgsensor_info.pre.pclk / framerate * 10 / imgsensor_info.pre.linelength;
        spin_lock(&imgsensor_drv_lock);
        imgsensor.dummy_line = (frame_length > imgsensor_info.pre.framelength) ? (frame_length - imgsensor_info.pre.framelength) : 0;
        imgsensor.frame_length = imgsensor_info.pre.framelength + imgsensor.dummy_line;
        imgsensor.min_frame_length = imgsensor.frame_length;
        spin_unlock(&imgsensor_drv_lock);
        //set_dummy();
        LOG_INF("error scenario_id = %d, we use preview scenario \n", scenario_id);
        break;
    }
    return ERROR_NONE;
}

static kal_uint32 get_default_framerate_by_scenario(MSDK_SCENARIO_ID_ENUM scenario_id, MUINT32* framerate)
{
    LOG_INF("scenario_id = %d\n", scenario_id);

    switch (scenario_id) {
    case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
        *framerate = imgsensor_info.pre.max_framerate;
        break;
    case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
        *framerate = imgsensor_info.normal_video.max_framerate;
        break;
    case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
        *framerate = imgsensor_info.cap.max_framerate;
        break;
    case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
        *framerate = imgsensor_info.hs_video.max_framerate;
        break;
    case MSDK_SCENARIO_ID_SLIM_VIDEO:
        *framerate = imgsensor_info.slim_video.max_framerate;
        break;
    default:
        break;
    }

    return ERROR_NONE;
}

static kal_uint32 set_test_pattern_mode(kal_bool enable)
{
    LOG_INF("enable: %d\n", enable);

    if (enable) {
        // 0x5E00[8]: 1 enable,  0 disable
        // 0x5E00[1:0]; 00 Color bar, 01 Random Data, 10 Square, 11 BLACK
        write_cmos_sensor(0x0600, 0x0002);
    }
    else {
        // 0x5E00[8]: 1 enable,  0 disable
        // 0x5E00[1:0]; 00 Color bar, 01 Random Data, 10 Square, 11 BLACK
        write_cmos_sensor(0x0600, 0x0000);
    }
    spin_lock(&imgsensor_drv_lock);
    imgsensor.test_pattern = enable;
    spin_unlock(&imgsensor_drv_lock);
    return ERROR_NONE;
}
static kal_uint32 feature_control(MSDK_SENSOR_FEATURE_ENUM feature_id,
    UINT8* feature_para, UINT32* feature_para_len)
{
    UINT16* feature_return_para_16 = (UINT16*)feature_para;
    UINT16* feature_data_16 = (UINT16*)feature_para;
    UINT32* feature_return_para_32 = (UINT32*)feature_para;
    UINT32* feature_data_32 = (UINT32*)feature_para;
    unsigned long long* feature_data = (unsigned long long*)feature_para;

    //SET_PD_BLOCK_INFO_T *PDAFinfo;
    SENSOR_WINSIZE_INFO_STRUCT* wininfo;

    MSDK_SENSOR_REG_INFO_STRUCT* sensor_reg_data = (MSDK_SENSOR_REG_INFO_STRUCT*)feature_para;

    LOG_INF("feature_id = %d\n", feature_id);
    switch (feature_id) {
    case SENSOR_FEATURE_GET_PERIOD:
        *feature_return_para_16++ = imgsensor.line_length;
        *feature_return_para_16 = imgsensor.frame_length;
        *feature_para_len = 4;
        break;
    case SENSOR_FEATURE_GET_PIXEL_CLOCK_FREQ:
        LOG_INF("feature_Control imgsensor.pclk = %d,imgsensor.current_fps = %d\n", imgsensor.pclk, imgsensor.current_fps);
        *feature_return_para_32 = imgsensor.pclk;
        *feature_para_len = 4;
        break;
    case SENSOR_FEATURE_SET_ESHUTTER:
        set_shutter(*feature_data);
        break;
    case SENSOR_FEATURE_SET_NIGHTMODE:
        //night_mode((BOOL) *feature_data);
        break;
    case SENSOR_FEATURE_SET_GAIN:
        set_gain((UINT16)*feature_data);
        break;
    case SENSOR_FEATURE_SET_FLASHLIGHT:
        break;
    case SENSOR_FEATURE_SET_ISP_MASTER_CLOCK_FREQ:
        break;
    case SENSOR_FEATURE_SET_REGISTER:
        write_cmos_sensor(sensor_reg_data->RegAddr, sensor_reg_data->RegData);
        break;
    case SENSOR_FEATURE_GET_REGISTER:
        sensor_reg_data->RegData = read_cmos_sensor(sensor_reg_data->RegAddr);
        break;
    case SENSOR_FEATURE_GET_LENS_DRIVER_ID:
        // get the lens driver ID from EEPROM or just return LENS_DRIVER_ID_DO_NOT_CARE
        // if EEPROM does not exist in camera module.
        *feature_return_para_32 = LENS_DRIVER_ID_DO_NOT_CARE;
        *feature_para_len = 4;
        break;
    case SENSOR_FEATURE_SET_VIDEO_MODE:
        set_video_mode(*feature_data);
        break;
    case SENSOR_FEATURE_CHECK_SENSOR_ID:
        get_imgsensor_id(feature_return_para_32);
        break;
    case SENSOR_FEATURE_SET_AUTO_FLICKER_MODE:
        set_auto_flicker_mode((BOOL)*feature_data_16, *(feature_data_16 + 1));
        break;
    case SENSOR_FEATURE_SET_MAX_FRAME_RATE_BY_SCENARIO:
        set_max_framerate_by_scenario((MSDK_SCENARIO_ID_ENUM)*feature_data, *(feature_data + 1));
        break;
    case SENSOR_FEATURE_GET_DEFAULT_FRAME_RATE_BY_SCENARIO:
        get_default_framerate_by_scenario((MSDK_SCENARIO_ID_ENUM) * (feature_data), (MUINT32*)(uintptr_t)(*(feature_data + 1)));
        break;
    case SENSOR_FEATURE_GET_PDAF_DATA:
        LOG_INF("SENSOR_FEATURE_GET_PDAF_DATA\n");
        read_3P3_eeprom((kal_uint16)(*feature_data), (char*)(uintptr_t)(*(feature_data + 1)), (kal_uint32)(*(feature_data + 2)));
        break;

    case SENSOR_FEATURE_SET_TEST_PATTERN:
        set_test_pattern_mode((BOOL)*feature_data);
        break;
    case SENSOR_FEATURE_GET_TEST_PATTERN_CHECKSUM_VALUE: //for factory mode auto testing
        *feature_return_para_32 = imgsensor_info.checksum_value;
        *feature_para_len = 4;
        break;
    case SENSOR_FEATURE_SET_FRAMERATE:
        LOG_INF("current fps :%d\n", (UINT32)*feature_data);
        spin_lock(&imgsensor_drv_lock);
        imgsensor.current_fps = *feature_data;
        spin_unlock(&imgsensor_drv_lock);
        break;
    case SENSOR_FEATURE_SET_HDR:
        LOG_INF("ihdr enable :%d\n", (BOOL)*feature_data);
        spin_lock(&imgsensor_drv_lock);
        imgsensor.ihdr_mode = *feature_data;
        spin_unlock(&imgsensor_drv_lock);
        break;
    case SENSOR_FEATURE_GET_CROP_INFO:
        LOG_INF("SENSOR_FEATURE_GET_CROP_INFO scenarioId:%d\n", (UINT32)*feature_data);
        wininfo = (SENSOR_WINSIZE_INFO_STRUCT*)(uintptr_t)(*(feature_data + 1));

        switch (*feature_data_32) {
        case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
            memcpy((void*)wininfo, (void*)&imgsensor_winsize_info[1], sizeof(SENSOR_WINSIZE_INFO_STRUCT));
            break;
        case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
            memcpy((void*)wininfo, (void*)&imgsensor_winsize_info[2], sizeof(SENSOR_WINSIZE_INFO_STRUCT));
            break;
        case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
            memcpy((void*)wininfo, (void*)&imgsensor_winsize_info[3], sizeof(SENSOR_WINSIZE_INFO_STRUCT));
            break;
        case MSDK_SCENARIO_ID_SLIM_VIDEO:
            memcpy((void*)wininfo, (void*)&imgsensor_winsize_info[4], sizeof(SENSOR_WINSIZE_INFO_STRUCT));
            break;
        case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
        default:
            memcpy((void*)wininfo, (void*)&imgsensor_winsize_info[0], sizeof(SENSOR_WINSIZE_INFO_STRUCT));
            break;
        }
        break;
    /*case SENSOR_FEATURE_GET_PDAF_INFO:
		LOG_INF("SENSOR_FEATURE_GET_PDAF_INFO scenarioId:%d\n", (UINT32)*feature_data);
		PDAFinfo= (SET_PD_BLOCK_INFO_T *)(uintptr_t)(*(feature_data+1));

		switch (*feature_data) {
			case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
				memcpy((void *)PDAFinfo,(void *)&imgsensor_pd_info,sizeof(SET_PD_BLOCK_INFO_T));
				break;
			case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
			case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
			case MSDK_SCENARIO_ID_SLIM_VIDEO:
			case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
			default:
				break;
		}
		break;
	case SENSOR_FEATURE_GET_SENSOR_PDAF_CAPACITY:
		LOG_INF("SENSOR_FEATURE_GET_SENSOR_PDAF_CAPACITY scenarioId:%d\n", (UINT32)*feature_data);
		//PDAF capacity enable or not, 2p8 only full size support PDAF
		switch (*feature_data) {
			case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
				*(MUINT32 *)(uintptr_t)(*(feature_data+1)) = 1;
				break;
			case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
				*(MUINT32 *)(uintptr_t)(*(feature_data+1)) = 0; // video & capture use same setting
				break;
			case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
				*(MUINT32 *)(uintptr_t)(*(feature_data+1)) = 0;
				break;
			case MSDK_SCENARIO_ID_SLIM_VIDEO:
				*(MUINT32 *)(uintptr_t)(*(feature_data+1)) = 0;
				break;
			case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
				*(MUINT32 *)(uintptr_t)(*(feature_data+1)) = 0;
				break;
			default:
				*(MUINT32 *)(uintptr_t)(*(feature_data+1)) = 0;
				break;
		}
		break;*/

    case SENSOR_FEATURE_SET_IHDR_SHUTTER_GAIN:
        LOG_INF("SENSOR_SET_SENSOR_IHDR LE=%d, SE=%d, Gain=%d\n", (UINT16)*feature_data, (UINT16) * (feature_data + 1), (UINT16) * (feature_data + 2));
        //ihdr_write_shutter_gain((UINT16)*feature_data,(UINT16)*(feature_data+1),(UINT16)*(feature_data+2));
        break;
    case SENSOR_FEATURE_SET_AWB_GAIN:
        break;
    case SENSOR_FEATURE_SET_HDR_SHUTTER:
        LOG_INF("SENSOR_FEATURE_SET_HDR_SHUTTER LE=%d, SE=%d\n", (UINT16)*feature_data, (UINT16) * (feature_data + 1));
        //ihdr_write_shutter((UINT16)*feature_data,(UINT16)*(feature_data+1));
        break;
    default:
        break;
    }

    return ERROR_NONE;
} /*	feature_control()  */

static SENSOR_FUNCTION_STRUCT sensor_func = {
    open,
    get_info,
    get_resolution,
    feature_control,
    control,
    close
};

//kin0603
UINT32 S5K3P3SX_MIPI_RAW_SensorInit(PSENSOR_FUNCTION_STRUCT* pfFunc)
//UINT32 IMX214_MIPI_SensorInit(PSENSOR_FUNCTION_STRUCT *pfFunc)
{
    /* To Do : Check Sensor status here */
    if (pfFunc != NULL)
        *pfFunc = &sensor_func;
    return ERROR_NONE;
} /*	OV5693_MIPI_RAW_SensorInit	*/