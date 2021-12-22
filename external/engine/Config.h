#pragma once

#if defined(_WIN32) || defined(__linux__)
#define ASSETS_FOLDER "./assets/"
#else
#define ASSETS_FOLDER ""
#endif

#define SHADERS_FOLDER ASSETS_FOLDER "shaders/"
#define FONTS_FOLDER   ASSETS_FOLDER "fonts/"
