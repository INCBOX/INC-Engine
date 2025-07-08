#include "engine_filesystem.h"
#include "filesystem.h"
#include "dll_exports.h"   // LibHandle, LoadLib, etc.
#include "dll_platform.h"
#include "dll_loader.h"
#include <iostream>

typedef IFileSystem* (*GetFileSystemInterfaceFn)();
static IFileSystem* g_pFS = nullptr;

bool LoadFileSystem(LibHandle dllHandle) {
    if (!dllHandle) {
        std::cerr << "[FileSystem] Null handle\n";
        return false;
    }

    auto GetFS = reinterpret_cast<GetFileSystemInterfaceFn>(GetLibProc(dllHandle, "GetFileSystemInterface"));
    if (!GetFS) {
        std::cerr << "[FileSystem] Failed to get GetFileSystemInterface()\n";
        return false;
    }

    g_pFS = GetFS();
    std::cout << "[FileSystem] Loaded IFileSystem from DLL\n";
    return true;
}

void UnloadFileSystem() {
    g_pFS = nullptr;
}

std::string FS_ResolvePath(const std::string& relative) {
    return g_pFS ? g_pFS->ResolvePath(relative) : "";
}
