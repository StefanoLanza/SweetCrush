#pragma once

#if _WIN32
#define ASSETS_FOLDER "./assets/"
#else
#define ASSETS_FOLDER ""
#endif

#define SHADERS_FOLDER ASSETS_FOLDER "shaders/"
#define FONTS_FOLDER ASSETS_FOLDER "fonts/"
