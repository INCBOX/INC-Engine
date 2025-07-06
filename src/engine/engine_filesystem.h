// filesystem.h
#pragma once

#include <string>

bool LoadFileSystem(void* dllHandle);
void UnloadFileSystem();

const std::string& FS_GetGameDir();
std::string FS_ResolvePath(const std::string& relative_path);