# Use the non-open-source part, if present
-include vendor/mediatek/tk6757_66_n1/BoardConfigVendor.mk

# Use the 6757 common part
include device/mediatek/mt6757/BoardConfig.mk

# OTA: Disable vendor partition
TARGET_COPY_OUT_VENDOR := system/vendor
BOARD_VENDORIMAGE_FILE_SYSTEM_TYPE :=
TARGET_RECOVERY_FSTAB := $(MTK_PTGEN_PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/etc/fstab.$(MTK_PLATFORM_DIR)

#Config partition size
-include $(MTK_PTGEN_OUT)/partition_size.mk
ifneq ($(strip $(MTK_AB_OTA_UPDATER)), yes)
BOARD_CACHEIMAGE_FILE_SYSTEM_TYPE := ext4
endif
BOARD_FLASH_BLOCK_SIZE := 4096
BOARD_MTK_SYSTEM_SIZE_KB := 2621440

MTK_INTERNAL_CDEFS := $(foreach t,$(AUTO_ADD_GLOBAL_DEFINE_BY_NAME),$(if $(filter-out no NO none NONE false FALSE,$($(t))),-D$(t)))
MTK_INTERNAL_CDEFS += $(foreach t,$(AUTO_ADD_GLOBAL_DEFINE_BY_VALUE),$(if $(filter-out no NO none NONE false FALSE,$($(t))),$(foreach v,$(shell echo $($(t)) | tr '[a-z]' '[A-Z]'),-D$(v))))
MTK_INTERNAL_CDEFS += $(foreach t,$(AUTO_ADD_GLOBAL_DEFINE_BY_NAME_VALUE),$(if $(filter-out no NO none NONE false FALSE,$($(t))),-D$(t)=\"$(strip $($(t)))\"))

MTK_GLOBAL_CFLAGS += $(MTK_INTERNAL_CDEFS)

ifneq ($(MTK_K64_SUPPORT), yes)
BOARD_KERNEL_CMDLINE = bootopt=64S3,32S1,32S1 androidboot.selinux=permissive
else
BOARD_KERNEL_CMDLINE = bootopt=64S3,32N2,64N2 androidboot.selinux=permissive
endif

MTK_RECOVERY_MEDIUM_RES := yes
