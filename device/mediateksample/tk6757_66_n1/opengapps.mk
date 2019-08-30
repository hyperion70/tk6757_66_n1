#
# OpenGApps AOSP based build system
#
# More details https://github.com/opengapps/aosp_build
#
#
###########################################
#
# NOTE:
# For MTK build with GAPPS see this guide https://blog.csdn.net/zengxx1989/article/details/79467527
#
###########################################


GAPPS_VARIANT := pico

GAPPS_FORCE_PACKAGE_OVERRIDES := true
GAPPS_FORCE_WEBVIEW_OVERRIDES := true
GAPPS_FORCE_BROWSER_OVERRIDES := true
GAPPS_FORCE_PIXEL_LAUNCHER := true

WITH_DEXPREOPT := true

$(call inherit-product-if-exists, vendor/opengapps/build/opengapps-packages.mk)

