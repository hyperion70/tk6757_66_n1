# Fingerprint

ifeq ($(strip $(MTK_FINGERPRINT_SUPPORT)), yes) 

PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.fingerprint.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.fingerprint.xml
# PRODUCT_COPY_FILES += device/mediateksample/tk6757_66_n1/init.gxfingerprint.rc:$(TARGET_COPY_OUT_VENDOR)/etc/init/hw/init.gxfingerprint.rc
DEVICE_MANIFEST_FILE += device/mediatek/common/project_manifest/manifest_fingerprint.xml

PRODUCT_PACKAGES += \
        android.hardware.biometrics.fingerprint@2.1-etservice \
        android.hardware.biometrics.fingerprint@2.1 \
        android.hardware.biometrics.fingerprint@2.1-service-custom \
        gx_fpd \
        init_goodix \
        fingerprint.default \
        libfpservice \
        libfp_client \
        gxfingerprint.default

PRODUCT_PROPERTY_OVERRIDES += \
		ro.hardware.fingerprint=gxfingerprint

$(shell mkdir -p out/target/product/$(MTK_TARGET_PROJECT)/vendor/lib)
$(shell ln -sf /system/lib/libbacktrace.so out/target/product/$(MTK_TARGET_PROJECT)/vendor/lib/libbacktrace.so)
$(shell ln -sf /system/lib/libdexfile.so out/target/product/$(MTK_TARGET_PROJECT)/vendor/lib/libdexfile.so)
$(shell ln -sf /system/lib/libstdc++.so out/target/product/$(MTK_TARGET_PROJECT)/vendor/lib/libstdc++.so)
$(shell ln -sf /system/lib/libunwind.so out/target/product/$(MTK_TARGET_PROJECT)/vendor/lib/libunwind.so)
$(shell mkdir -p out/target/product/$(MTK_TARGET_PROJECT)/vendor/lib64)
$(shell ln -sf /system/lib64/libbacktrace.so out/target/product/$(MTK_TARGET_PROJECT)/vendor/lib64/libbacktrace.so)
$(shell ln -sf /system/lib64/libdexfile.so out/target/product/$(MTK_TARGET_PROJECT)/vendor/lib64/libdexfile.so)
$(shell ln -sf /system/lib64/libstdc++.so out/target/product/$(MTK_TARGET_PROJECT)/vendor/lib64/libstdc++.so)
$(shell ln -sf /system/lib64/libunwind.so out/target/product/$(MTK_TARGET_PROJECT)/vendor/lib64/libunwind.so)

endif

