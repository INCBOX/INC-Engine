//
// launcher.cpp — INC Launcher (Source-style, cross-platform)
//
// Responsibilities:
// - Parses command-line arguments to find -game <modname>
// - Auto-detects game directory by locating gameinfo.txt if needed
// - Reads engine_path.txt to get DLL paths, with fallback defaults
// - Delegates filesystem path resolution to fs_launcher module
// - Calls PlatformHost_Run() to start main loop and engine
//
// Note:
// - No SDL or rendering code here — that belongs to platform_host and engine DLL.
//

#include "platform_host.h" 	// MAIN LOOP MODULE
#include "fs_launcher.h" 	// FILESYSTEM
#include "inc_dll_utils.h" // Defines LibHandle, LoadLib, etc. FILESYSTEM FOR


#if defined(_WIN32)
    #include <Windows.h>
#endif

#include <iostream>

using FSInitFn = bool(*)(const std::string&);

static void ShowError(const char* message) {
#if defined(_WIN32)
    MessageBoxA(nullptr, message, "Fatal Error", MB_ICONERROR);
#else
    std::cerr << "[Launcher] " << message << "\n";
#endif
}

int main(int argc, char* argv[]) {
    // Step 1: Resolve launch info (gameinfo path, DLL paths)
    auto launchInfoOpt = ResolveFilesystemLaunchInfo(argc, argv);
    if (!launchInfoOpt) {
        ShowError("Failed to resolve filesystem launch info");
        return -1;
    }

    const auto& info = *launchInfoOpt;

    // Step 2: Load the filesystem DLL
    LibHandle fsLib = LoadLib(info.fsDllPath.c_str());
    if (!fsLib) {
        ShowError("Failed to load filesystem DLL");
        return -2;
    }

    // Step 3: Load and call FS_Init
    auto FS_Init = reinterpret_cast<FSInitFn>(GetLibProc(fsLib, "FS_Init"));
    if (!FS_Init) {
        ShowError("FS_Init symbol not found in filesystem DLL");
        CloseLib(fsLib);
        return -3;
    }

    if (!FS_Init(info.gameinfoPath.string())) {
        ShowError("FS_Init failed during initialization");
        CloseLib(fsLib);
        return -3;
    }

    // Step 4: Run the engine
    int result = PlatformHost_Run(
        info.engineDllPath,
        fsLib,
        info.gameinfoPath.string(),
        "INC Engine"
    );

    // Step 5: Clean up
    CloseLib(fsLib);

    return result;
}