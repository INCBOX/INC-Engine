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
#include "dll_exports.h"   // LibHandle, LoadLib, etc.
#include "dll_platform.h"
#include "dll_loader.h"


#include "platform_host.h"   // MAIN LOOP MODULE
#include "fs_launcher.h"     // FILESYSTEM


#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include <string>
#include <optional>
#include <iostream>

// Show error message appropriately depending on platform
static void ShowError(const char* message) noexcept {
#if defined(_WIN32)
    MessageBoxA(nullptr, message, "Fatal Error", MB_ICONERROR | MB_OK);
#else
    std::cerr << "[Launcher] " << message << std::endl;
#endif
}

using FSInitFn = bool(*)(const std::string&);

int main(int argc, char* argv[]) {
    // Resolve launch information (gameinfo path, DLL paths)
    const auto launchInfoOpt = ResolveFilesystemLaunchInfo(argc, argv);
    if (!launchInfoOpt) {
        ShowError("Failed to resolve filesystem launch info");
        return EXIT_FAILURE;
    }

    const auto& info = *launchInfoOpt;

    // Load the filesystem DLL
    const LibHandle fsLib = LoadLib(info.fsDllPath.c_str());
    if (!fsLib) {
        ShowError("Failed to load filesystem DLL");
        return EXIT_FAILURE;
    }

    // Obtain FS_Init function pointer from the DLL
    const auto FS_Init = reinterpret_cast<FSInitFn>(GetLibProc(fsLib, "FS_Init"));
    if (!FS_Init) {
        ShowError("FS_Init symbol not found in filesystem DLL");
        CloseLib(fsLib);
        return EXIT_FAILURE;
    }

    // Initialize filesystem with gameinfo path
    if (!FS_Init(info.gameinfoPath.string())) {
        ShowError("FS_Init failed during initialization");
        CloseLib(fsLib);
        return EXIT_FAILURE;
    }

    // Run the engine main loop
    const int result = PlatformHost_Run(
        info.engineDllPath,
        fsLib,
        info.gameinfoPath.string(),
        "INC Engine"
    );

    // Clean up the filesystem DLL
    CloseLib(fsLib);

    return result;
}