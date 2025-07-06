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

#if defined(_WIN32)
    #include <Windows.h>
#endif

#include <iostream>

static void ShowError(const char* message) {
#if defined(_WIN32)
    MessageBoxA(NULL, message, "Fatal Error", MB_ICONERROR);
#else
    std::cerr << "[Launcher] " << message << "\n";
#endif
}

int main(int argc, char* argv[]) {
    auto launchInfoOpt = ResolveFilesystemLaunchInfo(argc, argv);
    if (!launchInfoOpt) {
        ShowError("Failed to resolve filesystem launch info");
        return -1;
    }

    const auto& info = *launchInfoOpt;

    return PlatformHost_Run(
        info.engineDllPath,
        info.fsDllPath,
        info.gameinfoPath.string(),
        "INC Engine"
    );
}