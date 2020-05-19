#
LOCAL_DIR := $(GET_LOCAL_DIR)
TARGET := tk6757_66_n1
MODULES += app/mt_boot \
           dev/lcm
PMIC_CHIP := MT6355
ifeq ($(findstring PMIC_CHIP, $(strip $(DEFINES))),)
DEFINES += PMIC_CHIP_$(shell echo $(PMIC_CHIP) | tr '[a-z]' '[A-Z]')
endif
MTK_EMMC_SUPPORT = yes
MTK_KERNEL_POWER_OFF_CHARGING = yes
DEFINES += MTK_NEW_COMBO_EMMC_SUPPORT
DEFINES += MTK_GPT_SCHEME_SUPPORT
MTK_PUMP_EXPRESS_PLUS_SUPPORT := yes
MTK_RT5081_PMU_CHARGER_SUPPORT := yes
MTK_RT5081_PMU_BLED_SUPPORT := yes
MTK_CHARGER_INTERFACE := yes
MTK_LCM_PHYSICAL_ROTATION = 0
CUSTOM_LK_LCM="td4310_fhd_dsi_vdo_chuangwei_malata"
MTK_SECURITY_SW_SUPPORT = yes
MTK_VERIFIED_BOOT_SUPPORT = yes
MTK_SEC_FASTBOOT_UNLOCK_SUPPORT = yes
BOOT_LOGO := M3_fhd
DEBUG := 0
DEFINES += WITH_DEBUG_UART=1
CUSTOM_LK_USB_UNIQUE_SERIAL=no
MTK_TINYSYS_SCP_SUPPORT = no
MTK_PROTOCOL1_RAT_CONFIG = C/Lf/Lt/W/T/G
MTK_DM_VERITY_OFF = yes
