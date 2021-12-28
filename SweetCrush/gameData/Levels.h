#pragma once

struct Level;

#if defined(_MSC_VER)
    //  Microsoft 
    #define DLL_EXPORT __declspec(dllexport)
    #define DLL_IMPORT __declspec(dllimport)
#elif defined(__GNUC__)
    //  GCC
    #define DLL_EXPORT __attribute__((visibility("default")))
    #define DLL_IMPORT
#else
    #define DLL_EXPORT
    #define DLL_IMPORT
#endif

extern "C" {

DLL_EXPORT int GetNumLevels();
DLL_EXPORT const Level& GetLevel(int levelIndex);

}
