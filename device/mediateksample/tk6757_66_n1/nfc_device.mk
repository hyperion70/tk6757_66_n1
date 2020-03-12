#####
##### NXP NFC Device Configuration makefile
######

NXP_NFC_PLATFORM := pn544
NXP_CHIP_TYPE := pn544
BOARD_NFC_DEVICE := "/dev/$(NXP_CHIP_TYPE)"

# These are the hardware-specific features
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/com.android.nfc_extras.xml:system/etc/permissions/com.android.nfc_extras.xml \
    frameworks/native/data/etc/android.hardware.nfc.hce.xml:system/etc/permissions/android.hardware.nfc.hce.xml \
    frameworks/native/data/etc/android.hardware.nfc.hcef.xml:system/etc/permissions/android.hardware.nfc.hcef.xml \
    frameworks/native/data/etc/com.nxp.mifare.xml:system/etc/permissions/com.nxp.mifare.xml \
    frameworks/native/data/etc/android.hardware.nfc.xml:system/etc/permissions/android.hardware.nfc.xml

# NFC config files
PRODUCT_COPY_FILES += \
     $(LOCAL_PATH)/nfc/etc/libnfc-brcm.conf:system/etc/libnfc-brcm.conf \
     $(LOCAL_PATH)/nfc/etc/nfcee_access.xml:system/etc/nfcee_access.xml \
     $(LOCAL_PATH)/nfc/etc/libnfc-nxp.conf:system/etc/libnfc-nxp.conf

# NFC stock blobs
PRODUCT_COPY_FILES += \
     $(LOCAL_PATH)/nfc/nfc_nci.pn54x.default.so:vendor/lib64/hw/nfc_nci.pn54x.default.so \
     $(LOCAL_PATH)/nfc/libpn548ad_fw.so:vendor/firmware/libpn548ad_fw.so

# NFC Init Files
PRODUCT_COPY_FILES += \
     $(LOCAL_PATH)/nfc/etc/init/init.pn54x.nfc.rc:vendor/etc/init/init.pn54x.nfc.rc

# NFC packages
PRODUCT_PACKAGES += \
    libnfc-nci \
    libnfc_nci_jni \
    NfcNci \
    com.android.nfc_extras \
    Tag \
    android.hardware.nfc@1.0-impl \
	android.hardware.nfc@1.0-service 

PRODUCT_PROPERTY_OVERRIDES += \
		ro.hardware.nfc_nci=pn54x.default

BOARD_SEPOLICY_DIRS += $(LOCAL_PATH)/nfc/sepolicy

DEVICE_MANIFEST_FILE += device/mediatek/common/project_manifest/manifest_nfc.xml

