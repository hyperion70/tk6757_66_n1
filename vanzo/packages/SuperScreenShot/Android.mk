LOCAL_PATH:= $(call my-dir)

LOCAL_PATH2:= $(LOCAL_PATH)

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_DEX_PREOPT := false

LOCAL_SRC_FILES := $(call all-java-files-under, src)

#LOCAL_JAVA_LIBRARIES += telephony-common voip-common
LOCAL_JAVA_LIBRARIES += mediatek-framework

LOCAL_PACKAGE_NAME := SuperScreenShot

LOCAL_CERTIFICATE := platform

include $(BUILD_PACKAGE)

include $(CLEAR_VARS)
include $(BUILD_MULTI_PREBUILT)
