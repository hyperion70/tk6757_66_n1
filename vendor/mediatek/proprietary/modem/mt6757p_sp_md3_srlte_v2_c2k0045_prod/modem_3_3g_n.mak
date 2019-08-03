#
#  Copyright Statement:
#  ---------------------------
#  This software/firmware and related documentation ("MediaTek Software") are
#  protected under relevant copyright laws. The information contained herein
#  is confidential and proprietary to MediaTek Inc. and/or its licensors.
#  Without the prior written permission of MediaTek inc. and/or its licensors,
#  any reproduction,modification, use or disclosure of MediaTek Software, and
#  information contained herein, in whole or in part, shall be strictly prohibited.
#
#  MediaTek Inc.(C)2016.All rights reserved.
#
#  BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND
#  AGREES THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK
#  SOFTWARE") RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED
#  TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
#  WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
#  WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
#  NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER
#  WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
#  INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER
#  AGREES TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING
#  THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE
#  RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
#  CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR
#  ANY MEDIATEK SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO
#  CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND
#  EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT
#  TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,AT MEDIATEK'S OPTION,
#  TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,OR REFUND ANY SOFTWARE
#  LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK
#  SOFTWARE AT ISSUE.
#
# *************************************************************************
#

USE_EHRPD=TRUE
USE_IMD_PSAVE=TRUE
CARRIER=OM
USE_IPV6=TRUE
USE_PA2_CTRL=TRUE
USE_APT=TRUE
USE_SUPA=FALSE
USE_32K_CLK_BYPASS=TRUE
USE_GSM_PROTECTION=TRUE
USE_IRAT=TRUE
USE_SRLTE=TRUE
  # Description:
  #   Switch of SRLTE.
  # Option Values:
  #   TRUE: Must be ture for SRLTE feature build.
  #   FALSE: Other than SRLTE feature build.
  # Switch-ability:
  #   Non-switchable
USE_SRLTE_PS_IT=TRUE
  # Description:
  #   The option is to enable SRLTE for C2K PS
  # Option Values:
  #   TRUE: enable SRLTE for C2K
  #   FALSE: disable SRLTE for C2K
  # Switch-ability:
  #   Non-switchable
USE_NO_IPV6=TRUE
XCO_TYPE=DCXO
HW_VERSION=P0
USE_IOPHAL_UART=FALSE
USE_PC_VOICE=TRUE
AUDIO_PATH_FEATURE=DEFAULT
USE_SIDB=TRUE
REMOVE_PWRDET=FALSE
USE_1X_ONLY=FALSE
USE_DIVERSITY_RF=FALSE
PACKAGE_DSP_IMG=TRUE
USE_SER_MUX=FALSE
USE_ATCMD_CH=TRUE
SUPPORT_BREW=FALSE
USE_MMC_DMA=FALSE
USE_USB=FALSE
DISABLE_SLEEP=FALSE
USE_NOR_FLASH=FALSE
USE_AP_INTERFACE=FALSE
IPC_PATH_FEATURE=CCIF
FLASH_LESS_EN=TRUE
USE_MTK_SDIO_SLAVE=FALSE
SO_EXTRA= SDIO_DEBUG IPC_DEBUG
USE_PSNOTOPEN=TRUE
USE_WATCHDOG=TRUE
RAM_DUMP_2_AP=TRUE
USE_CARDLOCK=FALSE
DSPV_IMAGE_TYPE=THIN_MODEM_CHINA_TELECOM
USE_GPS_TYPE=GPS_TYPE_EXTERNAL_ON_AP
USE_GPS_ON_AP_GPS_DEV0=TRUE
USE_RPC=TRUE
USE_GPS_RPC=TRUE
USE_AP_CALL_GPS=TRUE
CONFIG_AGPS=TRUE
USE_MTK_CBP=DEVELOPMENT
USE_OPTIMIZE_LOG=TRUE
USE_SECURITY_LOG=FALSE
  # Description:
  #   enable security logging check through ccirq msg from md1
  # Option Values:
  #   TRUE:  support securtiy logging check
  #   FALSE: not support this feature
  # Switch-ability
  #   Non-switchable
USE_UIM_POWER_SWITCH=TRUE
USE_MTK_SYS=FULL_SYSTEM
MTK_AUDIO_SUPPORT=TRUE
USE_OPTIMIZE_EXCEPTION=TRUE
USE_CUST_PLT=MT6757P_C2K_CUSTOM
USE_CCIRQ_PATH=CCIRQ
MODEM_IMAGE=modem_3_3g_n.img
AUTO_GEN_FSM_IMAGE=TRUE
  # Description:
  #   Whether to build generate fsm_rw_df*.img/fsm_rf_df*.img/fsm_cust_df*.img
  # Option Values:
  #   TRUE:  Must be true for normal MD3 build, to generate fsm_rw_df*.img/fsm_rf_df*.img/fsm_cust_df*.img
  #   FALSE: Used for SSDVT, Modem-only build, not generate fsm related images
  # Switch-ability
  #   Non-switchable

CHECK_IMG=FALSE
  # Description:
  #   Check the fsm images genarated in build with base fsm_image(fsm_image/<project>/)
  # Option Values:
  #   TRUE:  Check the fsm_images. If the fsm images are differnet, build fail.
  #   FALSE: Not check
  # Switch-ability
  #   Non-switchable

USE_RUIM=TRUE
R2B_3TXGAINS=FALSE
NO_MOIP=TRUE
USE_ONLY_AT=FALSE
CONFIG_UI=NONE
USE_ENHANCEDAT=TRUE
USE_LOG_SPLM=TRUE
  # Description:
  #   Enable New Log Feature: SPLM
  # Option Values:
  #   TRUE:  Enable SPLM Feature
  #   False: Disable SPLM Feature
  # Switch-ability:
  #   Non-switchable

USE_MEMORY_OPT=TRUE
  # Description:
  #   Memory Optimization,decrease memory size
  # Option Values:
  #   TRUE:  Enable Memory optimization change
  #   False: Disable Memory optimization change
  # Switch-ability:
  #   Non-switchable

USE_EE_PRODUCTION_MODE=TRUE
  # Description:
  #   Enable EE Dump Production Mode.
  # Option Values:
  #   TRUE:  Enable EE Dump Production Mode
  #   False: Disable EE Dump Production Mode
  # Switch-ability:
  #   Non-switchable

USE_IOP_CCIF=TRUE
  # Description:
  #  IOP base on CCIF
  # Option Values:
  #   TRUE:  Enable
  #   False: Disable
  # Switch-ability:
  #   Non-switchable

MTK_BT_CHIP = MTK_CONSYS_MT6757
  # Description:
  #    Set BT capability: SW/FW deocde/encode
  # Option Values:
  #    MTK_CONSYS_MT6735 :Need to support software BT codec (CVSD / mSBC).
  #    MTK_CONSYS_MT6755 :Need to support software BT codec (CVSD / mSBC).
  #    MTK_CONSYS_MT6757 :Need to support software BT codec (CVSD / mSBC).  
  #    MTK_MT6630:Need to support BT phone call PCM interface.
  #    NONE: Don't support BT
  # Switch-ability:
  #   Non-switchable


MTK_AUDIO_HIERARCHICAL_PARAM_SUPPORT = TRUE
  # Description:
  #    change mode and speech parameter with/without sate machine reset
  # Option Values:
  #   TRUE:  Enable
  #   FALSE: Disable
  # Switch-ability:
  #   Non-switchable

FIX_AFC_OFF = FALSE
  # Description:
  #    Set Fix AFC configuration
  # Option Values:
  #    TRUE : FIX AFC OFF
  #    FALSE : FIX AFC ON
  # Switch-ability:
  #   Non-switchable

MMAFC_SHARED = TRUE
  # Description:
  #    Set MMAFC configuration
  # Option Values:
  #    TRUE : MMAFC ON
  #    FALSE : MMAFC OFF
  # Switch-ability:
  #   Non-switchable

ORIGINAL_PROJECT_NAME = TK_MD_BASIC
  # Description:
  #   Project original name setting (for customer project reference only)
  # Option Values:
  #   N/A
  # Switch-ability:
  #   Non-switchable

ORIGINAL_FLAVOR = C2K_V2_6757
  # Description:
  #   Project original flavor setting (for customer project reference only)
  # Option Values:
  #   N/A
  # Switch-ability:
  #   Non-switchable

CUSTOM_FLAVOR = C2K_V2_6757
  # Description:
  #   custom folder flavor setting
  # Option Values:
  #   N/A
  # Switch-ability:
  #   Non-switchable

PLATFORM = MT6757P
  # Description:
  #   Name of BB-chip.
  # Option Values:
  #   NA:
  # Switch-ability:
  #   Non-switchable

BOARD_VER = MT6757_SP_MD3
  # Description:
  #   Name of the PCB or EVB.
  # Option Values:
  #   N/A
  # Switch-ability:
  #   Non-switchable

SUB_BOARD_VER = OLYMPUS
  # Description:
  #   Sub-name of the PCB or EVB.
  # Option Values:
  #   N/A
  # Switch-ability:
  #   Non-switchable

CHIP_VER = S00
  # Description:
  #   Chip version, changed according to ECO.
  # Option Values:
  #   S01: .
  #   S00: .
  #   S02: .
  # Switch-ability:
  #   Non-switchable

DEMO_PROJECT = TRUE
  # Description:
  #   To define projects that are experimental or for demo purposes only. It should NEVER be enabled on MP projects, and it is used by MTK internally. Customer does not need to change it
  # Option Values:
  #   TRUE: Enable this feature
  #   FALSE: Disable this feature
  # Switch-ability:
  #   Non-switchable

VENDOR = NONE
  # Description:
  #   If not NONE, the project is a vendor project or is doing vendor release
  # Option Values:
  #   (other than NONE): This project is for the specified VENDOR
  #   NONE: Not a vendor project
  # Switch-ability:
  #   Non-switchable

TEST_LOAD_TYPE = BASIC
  # Description:
  #   internal test load type
  # Option Values:
  #   L1S: L1S
  #   UDVT: UDVT
  #   BASIC: BASIC
  #   NONE: NONE
  # Switch-ability:
  #   Non-switchable

CHIP_VERSION_CHECK = FALSE
  # Description:
  #   Enable/Disable HW ID check and supported in all projects with all current online project
  # Option Values:
  #   TRUE: Enable this feature
  #   FALSE: Disable this feature
  # Switch-ability:
  #   [Any] -> [Any]

CHK_ENV_FLAG = TRUE
  # Description:
  #   to check build environment and tool chain version
  # Option Values:
  #   TRUE: need to check build environment and tool chain version;
  #         show error if the environment is not recommended.
  #   FALSE: need to check build environment and tool chain version;
  #          show WARNING if the environment is not recommended.
  # Switch-ability:
  #   [Any] -> [Any]

COMBO_MEMORY_SUPPORT = FALSE
  # Description:
  #   To support different memory part in the same load.
  # Option Values:
  #   TRUE: Enable combo memory support
  #   FALSE: Disable combo memory support.
  #          Default setting is FALSE.
  # Switch-ability:
  #   [Any] -> [Any]

CUSTOM_CFLAGS = -gdwarf-2
  # Description:
  #   Add custom cflag
  # Option Values:
  #   --debug --no_debug_macros: .
  # Switch-ability:
  #   Non-switchable

CUSTOM_DEBUG_MODULES =
  # Description:
  #   CUSTOM_DEBUG_MODULES means these modules will be built with debug information.
  # Option Values:
  #   NONE: Disable this feature
  #   ALL: all
  #   module_names: The modules' name that you want to turn on debug information, seperated by space
  # Switch-ability:
  #   [Any] -> [Any]

CUSTOM_NON_DEBUG_MODULES = NONE
  # Description:
  #   CUSTOM_NON_DEBUG_MODULES means it will be built without debug information after custom release
  # Option Values:
  #   NONE: .
  #   ALL: .
  #   module_names: Module name list using space seperated
  # Switch-ability:
  #   [Any] -> [Any]

RF_MODULE = MT6176
  # Description:
  #   describe the RF module used in this project
  # Option Values:
  #   MT6176: used for Jade (Orion+)
  # Switch-ability:
  #   [Any] -> [Any]

PMIC = MT6355
  # Description:
  #   To specify which Power Management IC(PMIC) is used, or no PMIC.
  # Option Values:
  #   MT6328: used for Denali1 & Denali3.
  #   MT6351: used for Jade.
  # Switch-ability:
  #   Non-switchable

RTOS = NUCLEUS
  # Description:
  #   The RTOS used in the load
  # Option Values:
  #   NUCLEUS_V2: Use Nucleus 2.x as the OX
  #   NUCLEUS: Use Nucles 1.13 as the OS
  # Switch-ability:
  #   Non-switchable

COMPILER = GCC
  # Description:
  #   RVCT or ADS
  # Option Values:
  #   ADS: Use ARM ADS tool chain.
  #   GCC: Use GNU GCC tool chain (Mentor Sourcery 2010.09 , 4.5.1)
  #   RVCT: Use ARM RVCT tool chain
  # Switch-ability:
  #   Non-switchable

DSP_SOLUTION = DSPM
  # Description:
  #   This option is used to configure whether DSP core is enabled by ARM. If  DSP core functionality is required, specify the suitable DSP type for this option. Otherwise, use NONE for this option
  # Option Values:
  #   DSPM: Teaklite related chips.
  #   NONE: .
  # Switch-ability:
  #   Non-switchable

DSP_IMG_FILE = dspm_mt6757p_ram_reva0_srlte.img
  # Description:
  #   To define DSPM image file name
  # Option Values:
  # Switch-ability:
  #   Non-switchable

RVCT_PARTIAL_LINK = FALSE
  # Description:
  #   True:turn on partial link; False:turn off partial link
  # Option Values:
  #   TRUE: In linking process, try normal link firstly, if OOM, try partiallink next
  #   FALSE: In linking process, normal link only
  # Switch-ability:
  #   Non-switchable

RVCT_VERSION = NONE
  # Description:
  #   ARM RVCT Version
  # Option Values:
  #   V31: ARM RVCT Version 3.1
  #   V22: ARM RVCT Version 2.2
  # Switch-ability:
  #   Non-switchable

USE_LOG_STATISTICS_PROFILE = TRUE
  # Description:
  #   log/data information report,information is stored in RAM, user needs analysis it.
  # Option Values:
  #   TRUE:  Enable  log/data information report
  #   FALSE: Disable  log/data information report
  # Switch-ability:
  #   Non-switchable

USE_LOG_TRACE_PROFILE = TRUE
  # Description:
  #   log/data information report,information is a normal trace.
  # Option Values:
  #   TRUE:  Enable  log/data information report
  #   FALSE: Disable  log/data information report
  # Switch-ability:
  #   Non-switchable

TX_TAS_ENABLE=TRUE
  # Description:
  #   Tx antenna selection for Single Antenna and Dual Antenna
  # Option Values:
  #   TRUE:  Enable TAS (Transmit Antenna Selection)
  #   FALSE: Disable TAS
  # Switch-ability:
  #   Non-switchable

EN_32KLESS=TRUE
  # Description:
  #   Enable 32k-less mode
  # Option Values:
  #   TRUE:  Enable 32k-less,make PHONE work properly even 32KHz XO is removed
  #   FALSE: Disable 32k-less,will make PHONE work improperly if 32KHz XO is removed
  # Switch-ability:
  #   Non-switchable

SPEECH_WW_OPERATOR_SCORE_ENHANCE = FALSE    
# Description:
# Enhance speech enhancement capability by operating in WB mode
# Option Values:
# TRUE : Enable Fixed WB enhancement (FWE)
# FALSE: Not Enable FWE
# Switch-ability:
# Non-switchable

USE_MULTICHANNEL=TRUE
  # Description:
  #   Enable multi-channel
  # Option Values:
  #   TRUE:  Enable multi-channel
  #   FALSE: Disable multi-channel
  # Switch-ability:
  #   Non-switchable

OPEN_DSP_SPEECH_SUPPORT = FALSE
# Description:
# Option Values:
# TRUE:SUPPORT OPEN DSP
# FALSE: SUPPORT FD216
# Switch-ability:
# Non-switchable

USE_CCISM = TRUE
  # Description:
  #   Enable/Disable CCISM feature. A conmunication mechanism, based on share memory, between C2K and SCP in Everest.
  # Option Values:
  #   TRUE: Enable this feature
  #   FALSE: Disable this feature
  # Switch-ability:
  #   Non-switchable

ETM_ENABLE = TRUE
# Description:
#   Enable ETM
# Option Values:
#   TRUE:  Enable ETM, will make PHONE supply PA Voltage with ETM other than PMU
#   FALSE: Disable ETM, will make PHONE supply PA Voltage with PMU other than ETM
# Switch-ability:
#   Non-switchable

PSO_MODE_SUPPORT = TRUE
  # Description:
  #   To support power shut off (PSO) feature, it must be valid when DISABLE_SLEEP=FALSE.
  # Option Values:
  #   TRUE: Enable PSO mode
  #   FALSE: Disable PSO made
  #          Default setting is FALSE.
  # Switch-ability:
  #   Non-switchable

BPI_VM_CTRL_ENABLE = FALSE
  # Description:
  # BPI ENABLE for PA_VM0 and PA_VM1
  # Option Values:
  # TRUE : Enable PA_VM0 and PA_VM1 
  # FALSE: Not Enable 
  # Switch-ability:
  # Non-switchable

OTP_SUPPORT = FALSE
  # Description:
  #   Support One-Time-Programming area for the flash device
  #   
  #   Notify:
  #   If  OTP_SUPPORT = TRUE
  #   
  #   Please also modify AP part : 
  #   <EMMC>
  #   Please set /mediatek/config/[project]/ProjectConfig.mk
  #   MTK_EMMC_SUPPORT_OTP = YES
  #   
  #   <NAND>
  #   Please set /mediatek/config/[project]/ProjectConfig.mk
  #   NAND_OTP_SUPPORT = YES
  #   
  #   Relative doc could be download from
  #   http://dms.mediatek.inc:80/webtop/drl/objectId/0900006f8056690f
  # Option Values:
  #   TRUE: Enable One-Time Program Support
  #   FALSE: Disable One-Time Program Support
  # Switch-ability:
  #  Non-switchable


# *************************************************************************
# Release Setting Section
# *************************************************************************
RELEASE_PACKAGE        = REL_COMP_CR_BASIC
RELEASE_PACKAGE_SUB_PS = REL_COMP_BASIC

RELEASE_$(strip $(INPUT_METHOD)) = SRC  # MTK/SRC, only works when INPUT_METHOD is turning on
RELEASE_INPUT_METHODS_SRC =   #
RELEASE_TYPE = NONE           # NONE, INTERNAL

# *************************************************************************
# Common definitions
# *************************************************************************
include make/${CORE_ROOT}/buildConfig.mak

# *************************************************************************
# Custom Release Component Configuration
# *************************************************************************

ifeq ($(strip $(CUSTOM_RELEASE)),TRUE)
  include make/${CORE_ROOT}/rel/$(strip $(RELEASE_PACKAGE)).mak
else
  include make/${CORE_ROOT}/rel/$(strip $(RELEASE_PACKAGE_SUB_PS)).mak
endif
