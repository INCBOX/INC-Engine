#pragma once
#include <string>
#include "dll_exports.h"
#include "dll_platform.h"  // for LibHandle, if you expose it publicly

class IFileSystem {
public:
    virtual ~IFileSystem() = default;
    virtual std::string ResolvePath(const std::string& relativePath) = 0;
};

// This is the exported getter used by engine or other DLLs to retrieve the interface.
extern "C" DLL_EXPORT IFileSystem* GetFileSystemInterface();
extern "C" DLL_EXPORT bool FS_Init(const std::string& path);
// extern "C" __declspec(dllexport) void FS_Shutdown(); // ADD THIS LATER FOR CLEANER EXIT..

extern LibHandle g_FileSystemLibHandle;