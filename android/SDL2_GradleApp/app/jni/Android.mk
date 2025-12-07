LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

#SDL_PATH := ../SDL  # SDL
#LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include  # SDL

# Add your application source files here...
LOCAL_SRC_FILES := $(wildcard $(LOCAL_PATH)/src/*.c) $(wildcard $(LOCAL_PATH)/src/*.cpp)

# List the shared libraries your 'main' module depends on.
# These names MUST match the LOCAL_MODULE names from the libraries' Android.mk files.
LOCAL_SHARED_LIBRARIES := SDL3 SDL3_mixer SDL3_image

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -lOpenSLES -llog -landroid
 # SDL

include $(BUILD_SHARED_LIBRARY)

# Import and build the dependency modules
$(call import-module, SDL)
$(call import-module, SDL_mixer)
$(call import-module, SDL_image)