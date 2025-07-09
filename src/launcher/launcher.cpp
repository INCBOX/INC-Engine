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

#include "runtime_kernel.h" 	// MAIN LOOP IS HERE
#include "fs_launcher.h"     	// FILESYSTEM

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
    // Resolve launch info as before...
    const auto launchInfoOpt = ResolveFilesystemLaunchInfo(argc, argv);
    if (!launchInfoOpt) {
        ShowError("Failed to resolve filesystem launch info");
        return EXIT_FAILURE;
    }
    const auto& info = *launchInfoOpt;

    RuntimeKernel kernel;
    if (!kernel.Initialize(info.fsDllPath, info.engineDllPath, info.gameinfoPath.string())) {
        ShowError("Runtime kernel failed initialization");
        return EXIT_FAILURE;
    }

    int result = kernel.RunMainLoop("INC Engine");
    kernel.Shutdown();

    return result;
}