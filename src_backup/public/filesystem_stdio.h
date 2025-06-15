#pragma once
#include <string>
#include <vector>

// Define DLL_EXPORT cross-platform
#if defined(_WIN32) || defined(_WIN64)
  #ifdef BUILDING_FILESYSTEM_DLL
    #define DLL_EXPORT __declspec(dllexport)
  #else
    #define DLL_EXPORT __declspec(dllimport)
  #endif
#else
  #define DLL_EXPORT
#endif

// C++-only API — don't wrap in extern "C"
DLL_EXPORT bool FS_Init(const std::string& gameinfo_path);
DLL_EXPORT std::string FS_ResolvePath(const std::string& relative_path);
DLL_EXPORT const std::string& FS_GetGameDir();
DLL_EXPORT const std::vector<std::string>& FS_GetSearchPaths();
