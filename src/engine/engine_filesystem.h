// filesystem.h
#pragma once

#include <string>

#include "dll_exports.h"   // LibHandle, LoadLib, etc.
#include "dll_platform.h"
#include "dll_loader.h"

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