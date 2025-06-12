#pragma once
#include <string>
#include <vector>

#ifdef _WIN32
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif

DLL_EXPORT bool FS_Init(const std::string& gameinfo_path);
DLL_EXPORT std::string FS_ResolvePath(const std::string& relative_path);
DLL_EXPORT const std::string& FS_GetGameDir();
DLL_EXPORT const std::vector<std::string>& FS_GetSearchPaths();

#ifdef __cplusplus
}
#endif
