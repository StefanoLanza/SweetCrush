#pragma once

#if defined(__ANDROID__) || defined(__OHOS__)
// On mobile phones, the content of the assets folder is packaged
#define ASSETS_FOLDER ""
#elif defined(_WIN32) || defined(__linux__)
#define ASSETS_FOLDER "./assets/"
#else
#define ASSETS_FOLDER ""
#endif

#define SHADERS_FOLDER ASSETS_FOLDER "shaders/"
#define FONTS_FOLDER   ASSETS_FOLDER "fonts/"

constexpr int instanceDataBufferSize = 64 * 1024;
