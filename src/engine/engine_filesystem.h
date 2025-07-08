// filesystem.h
#pragma once

#include <string>

#include "inc_dll_utils.h" // for LibHandle

#ifdef __cplusplus
extern "C" {
#endif

bool LoadFileSystem(LibHandle dllHandle);
void UnloadFileSystem();

const char* FS_GetGameDir();
std::string FS_ResolvePath(const std::string& relative_path);

#ifdef __cplusplus
}
#endif