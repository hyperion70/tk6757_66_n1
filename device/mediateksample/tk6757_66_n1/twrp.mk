# Release name
# PRODUCT_RELEASE_NAME := Santin_N1

RECOVERY_VARIANT := twrp
INTERNAL_LOCAL_CLANG_EXCEPTION_PROJECTS := external/busybox/
TARGET_RECOVERY_PIXEL_FORMAT := RGBA_8888
TARGET_RECOVERY_FSTAB := device/mediateksample/tk6757_66_n1/twrp.fstab
TW_CUSTOM_THEME := device/mediateksample/tk6757_66_n1/TW_theme
TW_EXCLUDE_SUPERSU := true
TW_INCLUDE_INJECTTWRP := false
TW_DEVICE_VERSION := OREO by hyperion70
TW_USE_MODEL_HARDWARE_ID_FOR_DEVICE_ID := true
TW_DEFAULT_LANGUAGE := ru
RECOVERY_SDCARD_ON_DATA := true
TW_DEFAULT_EXTERNAL_STORAGE := true
TW_BRIGHTNESS_PATH := /sys/class/leds/lcd-backlight/brightness
TW_MAX_BRIGHTNESS := 255
TW_ALWAYS_RMRF := false
TW_INCLUDE_CRYPTO := true
TW_CUSTOM_BATTERY_PATH := /sys/devices/platform/battery/power_supply/battery
TARGET_USE_CUSTOM_LUN_FILE_PATH := /sys/devices/platform/mt_usb/musb-hdrc.0.auto/gadget/lun%d/file
TW_HAS_MTP := true
TW_MTP_DEVICE := /dev/mtp_usb
TW_CUSTOM_CPU_TEMP_PATH := /sys/devices/virtual/thermal/thermal_zone1/temp

PRODUCT_DEFAULT_PROPERTY_OVERRIDES += ro.secure=0
PRODUCT_DEFAULT_PROPERTY_OVERRIDES += ro.debugable=1
PRODUCT_DEFAULT_PROPERTY_OVERRIDES += ro.adb.secure=0
PRODUCT_DEFAULT_PROPERTY_OVERRIDES += ro.allow.mock.location=0
PRODUCT_DEFAULT_PROPERTY_OVERRIDES += persist.sys.usb.config=mtp,adb
