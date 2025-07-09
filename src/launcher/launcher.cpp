//
// launcher.cpp — INC Launcher (Source-style, cross-platform)
//
// Responsibilities:
// - Resolves -game <mod> or gameinfo.txt
// - Loads fs_launcher + engine DLL
// - Delegates main loop to RuntimeKernel
//

#include "runtime_kernel.h"
#include "fs_launcher.h"

#if defined(_WIN32)
#include <windows.h>
#define SHOW_ERROR(msg) MessageBoxA(nullptr, msg, "Launcher Error", MB_ICONERROR | MB_OK)
#else
#include <iostream>
#define SHOW_ERROR(msg) std::cerr << "[Launcher] " << msg << std::endl
#endif

int main(int argc, char* argv[]) {
    auto launchInfo = ResolveFilesystemLaunchInfo(argc, argv);
    if (!launchInfo) {
        SHOW_ERROR("Could not resolve launch info");
        return 1;
    }

    RuntimeKernel kernel;
    if (!kernel.Initialize(launchInfo->fsDllPath, launchInfo->engineDllPath, launchInfo->gameinfoPath.string())) {
        SHOW_ERROR("Failed to initialize runtime kernel");
        return 1;
    }

    int result = kernel.RunMainLoop("INC Engine");
    kernel.Shutdown();
    return result;
}
