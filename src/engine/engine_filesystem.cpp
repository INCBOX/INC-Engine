// filesystem.cpp
#include "filesystem.h"
#include <Windows.h>
#include <iostream>

// DLL handle and function pointers
static HMODULE g_FileSystemDLL = nullptr;

typedef const std::string& (*FS_GetGameDirFn)();
typedef std::string (*FS_ResolvePathFn)(const std::string&);
static FS_GetGameDirFn FS_GetGameDirPtr = nullptr;
static FS_ResolvePathFn FS_ResolvePathPtr = nullptr;

bool LoadFileSystem(void* dllHandle) {
    g_FileSystemDLL = static_cast<HMODULE>(dllHandle);
    if (!g_FileSystemDLL) {
        std::cerr << "[FileSystem] Null handle passed to LoadFileSystem()\n";
        return false;
    }

    FS_GetGameDirPtr = reinterpret_cast<FS_GetGameDirFn>(GetProcAddress(g_FileSystemDLL, "FS_GetGameDir"));
    FS_ResolvePathPtr = reinterpret_cast<FS_ResolvePathFn>(GetProcAddress(g_FileSystemDLL, "FS_ResolvePath"));

    if (!FS_GetGameDirPtr || !FS_ResolvePathPtr) {
        std::cerr << "[FileSystem] Failed to resolve FileSystem exports\n";
        return false;
    }

    std::cout << "[FileSystem] FileSystem DLL interface loaded successfully\n";
    return true;
}


void UnloadFileSystem() {
    // Don't free the DLL here! Platform host owns DLL lifetime.
    FS_GetGameDirPtr = nullptr;
    FS_ResolvePathPtr = nullptr;
    std::cout << "[FileSystem] FileSystem interface pointers cleared\n";
}

const std::string& FS_GetGameDir() {
    if (FS_GetGameDirPtr) return FS_GetGameDirPtr();
    static std::string empty;
    return empty;
}

std::string FS_ResolvePath(const std::string& relative_path) {
    if (FS_ResolvePathPtr) return FS_ResolvePathPtr(relative_path);
    return "";
}
