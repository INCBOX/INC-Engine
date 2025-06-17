// launcher.cpp — INC Launcher (Source-style, cross-platform)
//
// Responsibilities:
// - Parses -game <modname> or auto-detects game directory
// - Reads engine_path.txt for DLL paths (optional fallback to defaults)
// - Loads filesystem_stdio dynamic library
// - Loads engine DLL / shared lib
// - Calls Engine_Run()
// No rendering or ShaderAPI logic here.

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <optional>

#if defined(_WIN32)
    #include <Windows.h>
    using LibHandle = HMODULE;
    #define LoadLib(path) LoadLibraryExA(path, NULL, LOAD_WITH_ALTERED_SEARCH_PATH)
    #define GetLibProc(lib, name) GetProcAddress(lib, name)
    #define CloseLib(lib) FreeLibrary(lib)
#else
    #include <dlfcn.h>
    #include <unistd.h>
    #include <limits.h>
    using LibHandle = void*;
    inline LibHandle LoadLib(const char* path) { return dlopen(path, RTLD_NOW); }
    inline void* GetLibProc(LibHandle lib, const char* name) { return dlsym(lib, name); }
    inline void CloseLib(LibHandle lib) { if (lib) dlclose(lib); }
#endif

// Function pointer types
using EngineRunFn = void(*)();
using FSInitFn = bool(*)(const std::string&);

// Helper: Get executable directory cross-platform
static std::optional<std::filesystem::path> GetExecutableDir() {
#if defined(_WIN32)
    char exePath[MAX_PATH] = {};
    if (!GetModuleFileNameA(NULL, exePath, sizeof(exePath))) {
        std::cerr << "[Launcher] GetModuleFileNameA failed\n";
        return std::nullopt;
    }
    return std::filesystem::path(exePath).parent_path();

#elif defined(__APPLE__)
    char exePath[PATH_MAX] = {};
    uint32_t size = sizeof(exePath);
    if (_NSGetExecutablePath(exePath, &size) != 0) {
        std::cerr << "[Launcher] _NSGetExecutablePath buffer too small\n";
        return std::nullopt;
    }
    return std::filesystem::path(exePath).parent_path();

#else // Linux
    char exePath[PATH_MAX] = {};
    ssize_t len = readlink("/proc/self/exe", exePath, sizeof(exePath) - 1);
    if (len == -1) {
        std::cerr << "[Launcher] readlink /proc/self/exe failed\n";
        return std::nullopt;
    }
    exePath[len] = '\0';
    return std::filesystem::path(exePath).parent_path();
#endif
}

// Helper: Parse engine_path.txt into vector<string>, ignoring empty lines and comments (#)
static std::vector<std::string> ParseEnginePathFile(const std::filesystem::path& filePath) {
    std::vector<std::string> dllPaths;
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "[Launcher] Could not open engine_path.txt, will use default DLL paths.\n";
        return dllPaths;
    }

    std::string line;
    while (std::getline(file, line)) {
        // Trim trailing/leading whitespace (simple)
        size_t start = line.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) continue;
        size_t end = line.find_last_not_of(" \t\r\n");
        std::string trimmed = line.substr(start, end - start + 1);
        if (trimmed.empty() || trimmed[0] == '#') continue; // Skip comments

        dllPaths.push_back(std::move(trimmed));
    }
    return dllPaths;
}

// Helper: Find platform DLL path from engine_path.txt entries like "win_engine=path"
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

// Show error in platform native way
static void ShowError(const char* message) {
#if defined(_WIN32)
    MessageBoxA(NULL, message, "Fatal Error", MB_ICONERROR);
#else
    std::cerr << "[Launcher] " << message << "\n";
#endif
}

int main(int argc, char* argv[]) {
    auto exeDirOpt = GetExecutableDir();
    if (!exeDirOpt) {
        ShowError("Failed to get executable directory");
        return -1;
    }
    const auto& exeDir = *exeDirOpt;

    // Parse -game argument (optional)
    std::filesystem::path gameDir;
    for (int i = 1; i < argc - 1; ++i) {
        if (std::string_view(argv[i]) == "-game") {
            gameDir = exeDir / argv[i + 1];
            break;
        }
    }

    // Auto-detect mod folder by presence of gameinfo.txt if no -game arg
    if (gameDir.empty()) {
        for (const auto& entry : std::filesystem::directory_iterator(exeDir)) {
            if (entry.is_directory()) {
                auto candidate = entry.path() / "gameinfo.txt";
                if (std::filesystem::exists(candidate)) {
                    gameDir = entry.path();
                    break;
                }
            }
        }
    }

    if (gameDir.empty()) {
        ShowError("Could not detect game directory with gameinfo.txt");
        return -2;
    }

    auto gameinfoPath = gameDir / "gameinfo.txt";
    if (!std::filesystem::exists(gameinfoPath)) {
        ShowError("Could not find gameinfo.txt in game directory");
        return -3;
    }

    std::cout << "[Launcher] Using gameinfo.txt: " << gameinfoPath << "\n";

    // Read engine_path.txt for DLL paths
    auto enginePathTxt = exeDir / "engine_path.txt";
    auto dllPaths = ParseEnginePathFile(enginePathTxt);

    // Determine platform-specific DLL paths, fallback to defaults
    std::optional<std::string> fsPathOpt, enginePathOpt;
#if defined(_WIN32)
    fsPathOpt = PickDllPath(dllPaths, "win_filesystem=");
    enginePathOpt = PickDllPath(dllPaths, "win_engine=");
    if (!fsPathOpt) fsPathOpt = (exeDir / "bin" / "filesystem_stdio.dll").string();
    if (!enginePathOpt) enginePathOpt = (exeDir / "bin" / "engine.dll").string();
#elif defined(__APPLE__)
    fsPathOpt = PickDllPath(dllPaths, "mac_filesystem=");
    enginePathOpt = PickDllPath(dllPaths, "mac_engine=");
    if (!fsPathOpt) fsPathOpt = (exeDir / "bin" / "libfilesystem_stdio.dylib").string();
    if (!enginePathOpt) enginePathOpt = (exeDir / "bin" / "libengine.dylib").string();
#else // Linux and others
    fsPathOpt = PickDllPath(dllPaths, "linux_filesystem=");
    enginePathOpt = PickDllPath(dllPaths, "linux_engine=");
    if (!fsPathOpt) fsPathOpt = (exeDir / "bin" / "libfilesystem_stdio.so").string();
    if (!enginePathOpt) enginePathOpt = (exeDir / "bin" / "libengine.so").string();
#endif

    const std::string& fsPath = *fsPathOpt;
    const std::string& enginePath = *enginePathOpt;

    std::cout << "[Launcher] Loading FileSystem DLL: " << fsPath << "\n";

    // Load filesystem_stdio library
    LibHandle fsLib = LoadLib(fsPath.c_str());
    if (!fsLib) {
        ShowError("Failed to load filesystem_stdio DLL");
        return -4;
    }

    // Get FS_Init function pointer
    auto FS_Init = reinterpret_cast<FSInitFn>(GetLibProc(fsLib, "FS_Init"));
    if (!FS_Init || !FS_Init(gameinfoPath.string())) {
        ShowError("Filesystem failed to initialize");
        CloseLib(fsLib);
        return -5;
    }

    std::cout << "[Launcher] Loading Engine DLL: " << enginePath << "\n";

    // Load engine library
    LibHandle engineLib = LoadLib(enginePath.c_str());
    if (!engineLib) {
        ShowError("Failed to load engine DLL");
        CloseLib(fsLib);
        return -6;
    }

    // Get Engine_Run function pointer
    auto Engine_Run = reinterpret_cast<EngineRunFn>(GetLibProc(engineLib, "Engine_Run"));
    if (!Engine_Run) {
        ShowError("Missing Engine_Run() in engine DLL");
        CloseLib(engineLib);
        CloseLib(fsLib);
        return -7;
    }

    // Run the engine (blocks until engine exits)
    Engine_Run();

    // Cleanup
    CloseLib(engineLib);
    CloseLib(fsLib);

    return 0;
}