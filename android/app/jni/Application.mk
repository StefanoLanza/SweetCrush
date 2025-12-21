# Use the modern, LLVM-based C++ standard library.
# This is required for all modern NDKs (r18+).
APP_STL := c++_shared

APP_SHORT_COMMANDS := true

# Optional but good practice: Define your target platform here
# This should match your app's minSdkVersion from build.gradle.
APP_PLATFORM := android-29

APP_CPPFLAGS += -std=c++17
APP_ABI := arm64-v8a armeabi-v7a x86_64
