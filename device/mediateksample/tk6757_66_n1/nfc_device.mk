#####
##### NXP NFC Device Configuration makefile
######

NXP_NFC_PLATFORM := pn54x

# These are the hardware-specific features
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.nfc.hce.xml:system/etc/permissions/android.hardware.nfc.hce.xml \
    frameworks/native/data/etc/android.hardware.nfc.hcef.xml:system/etc/permissions/android.hardware.nfc.hcef.xml \
    frameworks/native/data/etc/com.nxp.mifare.xml:system/etc/permissions/com.nxp.mifare.xml \
    frameworks/native/data/etc/android.hardware.nfc.xml:system/etc/permissions/android.hardware.nfc.xml

# NFC config files
PRODUCT_COPY_FILES += \
     $(LOCAL_PATH)/nfc/etc/libnfc-brcm.conf:vendor/etc/libnfc-brcm.conf \
     $(LOCAL_PATH)/nfc/etc/libnfc-nxp_RF.conf:vendor/etc/libnfc-nxp_RF.conf \
     $(LOCAL_PATH)/nfc/etc/libnfc-nxp.conf:vendor/etc/libnfc-nxp.conf

# NFC Init Files
PRODUCT_COPY_FILES += \
     $(LOCAL_PATH)/nfc/etc/init/init.pn54x.nfc.rc:vendor/etc/init/init.$(NXP_NFC_PLATFORM).nfc.rc

# NFC packages
PRODUCT_PACKAGES += \
    libnfc-nci \
    libnfc_nci_jni \
    NfcNci \
    com.android.nfc_extras \
    nfcservice \
    nfc_nci.$(NXP_NFC_PLATFORM).default \
    Tag \
    android.hardware.nfc@1.0-impl \

PRODUCT_PACKAGES += \
	android.hardware.nfc@1.0-service \

PRODUCT_PACKAGES += \
	vendor.nxp.nxpnfc@1.0-impl \
	vendor.nxp.nxpnfc@1.0-service


PRODUCT_PROPERTY_OVERRIDES += \
		ro.hardware.nfc_nci=$(NXP_NFC_PLATFORM) 

PRODUCT_PROPERTY_OVERRIDES += \
        ro.product.first_api_level=23

DEVICE_MANIFEST_FILE += device/mediatek/common/project_manifest/manifest_nfc.xml

