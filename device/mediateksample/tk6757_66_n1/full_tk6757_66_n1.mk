# Inherit for devices that support 64-bit primary and 32-bit secondary zygote startup script
$(call inherit-product, $(SRC_TARGET_DIR)/product/core_64_bit.mk)

# Inherit from those products. Most specific first.
#$(call inherit-product, $(SRC_TARGET_DIR)/product/full_base_telephony.mk)
# Inherit from those products. Most specific first.
$(call inherit-product, $(SRC_TARGET_DIR)/product/full_base.mk)

MTK_BUILD_GAPPS = yes

# Set target and base project for flavor build
MTK_TARGET_PROJECT := $(subst full_,,$(TARGET_PRODUCT))
MTK_BASE_PROJECT := $(MTK_TARGET_PROJECT)
MTK_PROJECT_FOLDER := $(LOCAL_PATH)
MTK_TARGET_PROJECT_FOLDER := $(LOCAL_PATH)

# This is where we'd set a backup provider if we had one
#$(call inherit-product, device/sample/products/backup_overlay.mk)
$(call inherit-product, $(LOCAL_PATH)/device.mk)

# set locales & aapt config.
include $(LOCAL_PATH)/ProjectConfig.mk
PRODUCT_LOCALES:=ru_RU en_US


# Set those variables here to overwrite the inherited values.
PRODUCT_MANUFACTURER := Santin
PRODUCT_NAME := full_tk6757_66_n1
PRODUCT_DEVICE := tk6757_66_n1
PRODUCT_MODEL := Santin_N1
PRODUCT_POLICY := android.policy_phone
PRODUCT_BRAND := Santin
TARGET_OTA_ASSERT_DEVICE := N1

ifeq ($(TARGET_BUILD_VARIANT), eng)
KERNEL_DEFCONFIG ?= tk6757_66_n1_debug_defconfig
else
KERNEL_DEFCONFIG ?= tk6757_66_n1_defconfig
endif
PRELOADER_TARGET_PRODUCT ?= tk6757_66_n1
LK_PROJECT ?= tk6757_66_n1
TRUSTY_PROJECT ?= tk6757_66_n1


ifeq ($(MTK_BUILD_GAPPS),yes)
$(call inherit-product, device/mediateksample/tk6757_66_n1/opengapps.mk)

BUILD_FINGERPRINT := "Xiaomi/sirius/sirius:8.1.0/OPM1.171019.019/V9.5.9.0.OEBCNFA:user/release-keys"
PRIVATE_BUILD_DESC := "sirius-user 8.1.0 OPM1.171019.019 V9.5.9.0.OEBCNFA release-keys"

endif
