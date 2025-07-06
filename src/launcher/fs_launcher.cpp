#include "fs_launcher.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <optional>
#include <filesystem>

#if defined(_WIN32)
    #include <Windows.h>
#elif defined(__APPLE__)
    #include <mach-o/dyld.h>
    #include <limits.h>
#else
    #include <unistd.h>
    #include <limits.h>
#endif

namespace fs = std::filesystem;

static std::optional<fs::path> GetExecutableDir() {
#if defined(_WIN32)
    char exePath[MAX_PATH] = {};
    if (!GetModuleFileNameA(NULL, exePath, sizeof(exePath))) {
        std::cerr << "[INC_FILESYS] GetModuleFileNameA failed\n";
        return std::nullopt;
    }
    return fs::path(exePath).parent_path();
#elif defined(__APPLE__)
    char exePath[PATH_MAX] = {};
    uint32_t size = sizeof(exePath);
    if (_NSGetExecutablePath(exePath, &size) != 0) {
        std::cerr << "[INC_FILESYS] _NSGetExecutablePath buffer too small\n";
        return std::nullopt;
    }
    return fs::path(exePath).parent_path();
#else
    char exePath[PATH_MAX] = {};
    ssize_t len = readlink("/proc/self/exe", exePath, sizeof(exePath) - 1);
    if (len == -1) {
        std::cerr << "[INC_FILESYS] readlink /proc/self/exe failed\n";
        return std::nullopt;
    }
    exePath[len] = '\0';
    return fs::path(exePath).parent_path();
#endif
}

static std::string Trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    size_t last = str.find_last_not_of(" \t\r\n");
    return (first == std::string::npos) ? "" : str.substr(first, last - first + 1);
}

static std::vector<std::string> ParseEnginePathFile(const fs::path& filePath) {
    std::vector<std::string> dllPaths;
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "[INC_FILESYS] Could not open engine_path.txt: " << filePath << "\n";
        return dllPaths;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::string trimmed = Trim(line);
        if (trimmed.empty() || trimmed[0] == '#') continue;
        dllPaths.push_back(std::move(trimmed));
    }
    return dllPaths;
}

static std::optional<std::string> PickDllPath(const std::vector<std::string>& dllPaths, const std::string& prefix) {
    for (const auto& path : dllPaths) {
        if (path.compare(0, prefix.size(), prefix) == 0) {
            auto pos = path.find('=');
            if (pos != std::string::npos && pos + 1 < path.size()) {
                return path.substr(pos + 1);
            }
        }
    }
    return std::nullopt;
}

std::optional<FsLaunchInfo> ResolveFilesystemLaunchInfo(int argc, char* argv[]) {
    FsLaunchInfo info;

    auto exeDirOpt = GetExecutableDir();
    if (!exeDirOpt) {
        std::cerr << "[INC_FILESYS] Failed to get executable directory\n";
        return std::nullopt;
    }
    info.exeDir = *exeDirOpt;

    // Parse -game argument
    for (int i = 1; i < argc - 1; ++i) {
        if (std::string_view(argv[i]) == "-game") {
            info.gameDir = info.exeDir / argv[i + 1];
            break;
        }
    }

    // Auto-detect game directory by finding gameinfo.txt if not specified
    if (info.gameDir.empty()) {
        for (const auto& entry : fs::directory_iterator(info.exeDir)) {
            if (entry.is_directory()) {
                auto candidate = entry.path() / "gameinfo.txt";
                if (fs::exists(candidate)) {
                    info.gameDir = entry.path();
                    break;
                }
            }
        }
    }

    if (info.gameDir.empty()) {
        std::cerr << "[INC_FILESYS] Could not detect game directory with gameinfo.txt\n";
        return std::nullopt;
    }

    info.gameinfoPath = info.gameDir / "gameinfo.txt";
    if (!fs::exists(info.gameinfoPath)) {
        std::cerr << "[INC_FILESYS] gameinfo.txt not found in game directory: " << info.gameinfoPath << "\n";
        return std::nullopt;
    }

    std::cout << "[INC_FILESYS] Using gameinfo.txt: " << info.gameinfoPath << "\n";

    // Parse engine_path.txt
    auto enginePathTxt = info.exeDir / "engine_path.txt";
    auto dllPaths = ParseEnginePathFile(enginePathTxt);

#if defined(_WIN32)
    auto fsPathOpt = PickDllPath(dllPaths, "win_filesystem=");
    auto enginePathOpt = PickDllPath(dllPaths, "win_engine=");
    if (!fsPathOpt) fsPathOpt = (info.exeDir / "bin" / "filesystem_stdio.dll").string();
    if (!enginePathOpt) enginePathOpt = (info.exeDir / "bin" / "engine.dll").string();
#elif defined(__APPLE__)
    auto fsPathOpt = PickDllPath(dllPaths, "mac_filesystem=");
    auto enginePathOpt = PickDllPath(dllPaths, "mac_engine=");
    if (!fsPathOpt) fsPathOpt = (info.exeDir / "bin" / "libfilesystem_stdio.dylib").string();
    if (!enginePathOpt) enginePathOpt = (info.exeDir / "bin" / "libengine.dylib").string();
#else
    auto fsPathOpt = PickDllPath(dllPaths, "linux_filesystem=");
    auto enginePathOpt = PickDllPath(dllPaths, "linux_engine=");
    if (!fsPathOpt) fsPathOpt = (info.exeDir / "bin" / "libfilesystem_stdio.so").string();
    if (!enginePathOpt) enginePathOpt = (info.exeDir / "bin" / "libengine.so").string();
#endif

    if (!fsPathOpt || !enginePathOpt) {
        std::cerr << "[INC_FILESYS] Failed to determine DLL paths\n";
        return std::nullopt;
    }

    info.fsDllPath = *fsPathOpt;
    info.engineDllPath = *enginePathOpt;

    std::cout << "[INC_FILESYS] Filesystem DLL path: " << info.fsDllPath << "\n";
    std::cout << "[INC_FILESYS] Engine DLL path: " << info.engineDllPath << "\n";

    return info;
}
