LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := engine

#LOCAL_C_INCLUDES := $(LOCAL_PATH)

#LOCAL_SRC_FILES := Maths.cpp

# TODO Exclude linux and windows folders
LOCAL_SRC_FILES :=                                      \
    $(subst $(LOCAL_PATH)/,,                            \
      $(wildcard $(LOCAL_PATH)/*.cpp)                 \
    )

#LOCAL_CFLAGS :=
#LOCAL_LDLIBS :=
#LOCAL_LDFLAGS := -Wl,--no-undefined -Wl,
#LOCAL_STATIC_LIBRARIES := 
LOCAL_SHARED_LIBRARIES := SDL3 SDL3_mixer SDL3_image inih
LOCAL_LDLIBS := -lGLESv3
LOCAL_EXPORT_C_INCLUDES += $(LOCAL_PATH)/..

include $(BUILD_SHARED_LIBRARY)
