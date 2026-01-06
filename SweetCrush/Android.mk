LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := game

LOCAL_SRC_FILES := $(subst $(LOCAL_PATH)/,, $(wildcard $(LOCAL_PATH)/src/*.cpp))
LOCAL_SRC_FILES += $(subst $(LOCAL_PATH)/,, $(wildcard $(LOCAL_PATH)/gameData/*.cpp))

LOCAL_CFLAGS :=
LOCAL_LDLIBS :=
LOCAL_LDFLAGS := -Wl,--no-undefined -Wl,
LOCAL_SHARED_LIBRARIES := SDL3 SDL3_mixer SDL3_image engine

include $(BUILD_SHARED_LIBRARY)
