PRODUCT_PACKAGES += $(VANZO_OVERSEAS_CUSTOM_APPS)
#Vanzo:renmengyang on: Tue, 18 Jul 2017 16:36:49 +0800
#add aging test
ifeq ($(VANZO_FEATURE_AGING_TEST), yes)
        PRODUCT_PACKAGES += BeeTest
endif
#End of Vanzo:renmengyang

# PRODUCT_PACKAGES += FactoryMode
ifeq ($(strip $(VANZO_FEATURE_ADD_GM_SERVICE)), yes)
    PRODUCT_PACKAGES += GmService
endif

#fota start
ifeq ($(strip $(ADUPS_FOTA_SUPPORT)), yes)
    PRODUCT_PACKAGES += AdupsFota \
                    AdupsFotaReboot
endif

#fota end
ifeq ($(strip $(VANZO_FEATURE_SOUNDRECORD_SUPPORT_O0)), yes)
    PRODUCT_PACKAGES += VanzoSoundRecorder
endif

ifeq ($(strip $(VANZO_FEATURE_SOUNDRECORD_SUPPORT_O1)), yes)
    PRODUCT_PACKAGES += VanzoSoundRecorder
endif

ifeq ($(strip $(VANZO_FEATURE_RUNTIME_TEST)), yes)
    PRODUCT_PACKAGES += RuntimeTest
endif
ifneq ($(strip $(VANZO_FEATURE_CUSTOM_CHROME_HOMEPAGE)), "")
    PRODUCT_PACKAGES += ChromeCustomizations
endif
ifeq ($(strip $(VANZO_FEATURE_SETTINGS_QRCODE)), yes)
  PRODUCT_PACKAGES += QRcode
endif

# Vanzo:songshuangwen on: Sun, 18 Mar 2018 13:21:44 +0800
# add SuperScreenShot
ifeq ($(strip $(VANZO_FEATURE_SYSTEMUI_ADD_SUPERSCREENSHOT)), yes)
  PRODUCT_PACKAGES += SuperScreenShot
endif
# End of Vanzo:songshuangwen

# Vanzo:hanshengfei on: Mon, 05 Mar 2018 16:16:03 +0800
# add GangYunCamera
ifeq ($(VANZO_FEATURE_GANGYUN_CAMERA_SUPPORT), yes)
  PRODUCT_PACKAGES += GangYunCamera
endif
ifeq ($(strip $(GANGYUN_BOKEH_SUPPORT)), yes)
  PRODUCT_PACKAGES += BeautySnap
endif
ifeq ($(strip $(GANGYUN_CAMERA_BEAUTY)), yes)
  PRODUCT_PACKAGES += BeautySnap
endif
# End of Vanzo:hanshengfei
