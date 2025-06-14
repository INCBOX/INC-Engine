// launcher.cpp — INC Launcher (Source-style, cross-platform)
//
// Responsibilities:
// - Parses -game <modname> or auto-detects game directory
// - Reads engine_path.txt to determine DLL paths (optional fallback to defaults)
// - Loads filesystem_stdio as dynamic library
// - Loads engine.dll / libengine.so
// - Calls Engine_Run()
// This file contains NO rendering or ShaderAPI logic.

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>

#if defined(_WIN32)
    #include <Windows.h>
    typedef void (__stdcall *EngineRunFn)();
    typedef bool (*FSInitFn)(const std::string&);
#else
    #include <dlfcn.h>
    #include <unistd.h> // for readlink
    typedef void (*EngineRunFn)();
    typedef bool (*FSInitFn)(const std::string&);
#endif

// Helper: Get the directory where the launcher executable lives
static std::string GetExecutableDir() {
#if defined(_WIN32)
    char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, MAX_PATH);
#else
    char exePath[1024];
    ssize_t len = readlink("/proc/self/exe", exePath, sizeof(exePath) - 1);
    if (len != -1) exePath[len] = '\0';
    else strcpy(exePath, "./");
#endif
    return std::filesystem::path(exePath).parent_path().string();
}

// Helper: Parse engine_path.txt lines into a vector of paths
static std::vector<std::string> ParseEnginePathFile(const std::string& filePath) {
    std::vector<std::string> dllPaths;
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "[Launcher] Could not open engine_path.txt, will use default DLL paths.\n";
        return dllPaths;
    }
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty())
            dllPaths.push_back(line);
    }
    return dllPaths;
}

// Helper: Pick DLL path from engine_path.txt based on current platform prefix
static std::string PickDllPath(const std::vector<std::string>& dllPaths, const std::string& prefix) {
    for (const auto& path : dllPaths) {
        if (path.rfind(prefix, 0) == 0) { // line starts with prefix
            auto pos = path.find('=');
            if (pos != std::string::npos && pos + 1 < path.size())
                return path.substr(pos + 1);
        }
    }
    return {}; // not found
}

int main(int argc, char** argv) {
    std::string exeDir = GetExecutableDir();
    std::string gameDir;

    // Step 1: Parse -game <modname> argument (optional)
    for (int i = 0; i < argc - 1; ++i) {
        if (std::string(argv[i]) == "-game") {
            gameDir = (std::filesystem::path(exeDir) / argv[i + 1]).string();
            break;
        }
    }

    // Step 2: Auto-detect mod folder with gameinfo.txt if -game not given
    if (gameDir.empty()) {
        for (const auto& entry : std::filesystem::directory_iterator(exeDir)) {
            if (entry.is_directory()) {
                std::string candidate = (entry.path() / "gameinfo.txt").string();
                if (std::filesystem::exists(candidate)) {
                    gameDir = entry.path().string();
                    break;
                }
            }
        }
    }

    // Step 3: Validate gameinfo.txt exists
    std::string gameinfoPath = (std::filesystem::path(gameDir) / "gameinfo.txt").string();
    if (!std::filesystem::exists(gameinfoPath)) {
#if defined(_WIN32)
        MessageBoxA(NULL, "Could not find gameinfo.txt", "Fatal Error", MB_ICONERROR);
#else
        std::cerr << "[Launcher] Could not find gameinfo.txt\n";
#endif
        return -1;
    }

    std::cout << "[Launcher] Using gameinfo.txt: " << gameinfoPath << "\n";

    // Step 4: Read engine_path.txt to get DLL paths (optional)
    std::string enginePathTxt = (std::filesystem::path(exeDir) / "engine_path.txt").string();
    auto dllPaths = ParseEnginePathFile(enginePathTxt);

    // Step 5: Pick platform-specific DLL paths from engine_path.txt, fallback to defaults if missing
    std::string fsPath, enginePath;

#if defined(_WIN32)
    fsPath = PickDllPath(dllPaths, "win_filesystem=");
    enginePath = PickDllPath(dllPaths, "win_engine=");
    if (fsPath.empty()) fsPath = (std::filesystem::path(exeDir) / "bin" / "filesystem_stdio.dll").string();
    if (enginePath.empty()) enginePath = (std::filesystem::path(exeDir) / "bin" / "engine.dll").string();
#elif defined(__APPLE__)
    fsPath = PickDllPath(dllPaths, "mac_filesystem=");
    enginePath = PickDllPath(dllPaths, "mac_engine=");
    if (fsPath.empty()) fsPath = (std::filesystem::path(exeDir) / "bin" / "libfilesystem_stdio.dylib").string();
    if (enginePath.empty()) enginePath = (std::filesystem::path(exeDir) / "bin" / "libengine.dylib").string();
#else // Linux and others
    fsPath = PickDllPath(dllPaths, "linux_filesystem=");
    enginePath = PickDllPath(dllPaths, "linux_engine=");
    if (fsPath.empty()) fsPath = (std::filesystem::path(exeDir) / "bin" / "libfilesystem_stdio.so").string();
    if (enginePath.empty()) enginePath = (std::filesystem::path(exeDir) / "bin" / "libengine.so").string();
#endif

    std::cout << "[Launcher] Loading FileSystem DLL: " << fsPath << "\n";

    // Step 6: Load filesystem_stdio DLL
#if defined(_WIN32)
    HMODULE fsLib = LoadLibraryExA(fsPath.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
#else
    void* fsLib = dlopen(fsPath.c_str(), RTLD_NOW);
#endif
    if (!fsLib) {
#if defined(_WIN32)
        MessageBoxA(NULL, "Failed to load filesystem_stdio DLL", "Fatal Error", MB_ICONERROR);
#else
        std::cerr << "[Launcher] Failed to load FileSystem DLL: " << fsPath << "\n";
#endif
        return -2;
    }

    // Step 7: Get FS_Init function pointer and initialize filesystem with gameinfo.txt
#if defined(_WIN32)
    FSInitFn FS_Init = (FSInitFn)GetProcAddress(fsLib, "FS_Init");
#else
    FSInitFn FS_Init = (FSInitFn)dlsym(fsLib, "FS_Init");
#endif
    if (!FS_Init || !FS_Init(gameinfoPath)) {
#if defined(_WIN32)
        MessageBoxA(NULL, "Filesystem failed to initialize", "Fatal Error", MB_ICONERROR);
#else
        std::cerr << "[Launcher] FileSystem initialization failed\n";
#endif
        return -3;
    }

    std::cout << "[Launcher] Loading Engine DLL: " << enginePath << "\n";

    // Step 8: Load engine DLL
#if defined(_WIN32)
    HMODULE engineLib = LoadLibraryExA(enginePath.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
#else
    void* engineLib = dlopen(enginePath.c_str(), RTLD_NOW);
#endif
    if (!engineLib) {
#if defined(_WIN32)
        MessageBoxA(NULL, "Failed to load engine DLL", "Fatal Error", MB_ICONERROR);
#else
        std::cerr << "[Launcher] Failed to load Engine DLL: " << enginePath << "\n";
#endif
        return -4;
    }

    // Step 9: Get Engine_Run function pointer
#if defined(_WIN32)
    EngineRunFn Engine_Run = (EngineRunFn)GetProcAddress(engineLib, "Engine_Run");
#else
    EngineRunFn Engine_Run = (EngineRunFn)dlsym(engineLib, "Engine_Run");
#endif
    if (!Engine_Run) {
#if defined(_WIN32)
        MessageBoxA(NULL, "Missing Engine_Run() in engine DLL", "Fatal Error", MB_ICONERROR);
#else
        std::cerr << "[Launcher] Engine_Run not found in engine DLL\n";
#endif
        return -5;
    }

    // Step 10: Call Engine_Run main loop
    Engine_Run();

    return 0;
}