//
// launcher.cpp — INC Bootstrapper (modular, Source-style)
//

#include "fs_launcher.h"
#include "runtime_kernel.h"

#include <string>
#include <sstream>

#if defined(_WIN32)
    #include <windows.h>
    #define LOG(msg) OutputDebugStringA((msg).c_str())
    #define SHOW_ERROR(msg) MessageBoxA(nullptr, msg, "INC Bootstrapper", MB_ICONERROR | MB_OK)
#else
    #include <iostream>
    #define LOG(msg) std::cout << "[Bootstrapper] " << msg << std::endl
    #define SHOW_ERROR(msg) std::cerr << "[Bootstrapper Error] " << msg << std::endl
#endif

int main(int argc, char* argv[]) {
    auto launchInfo = ResolveFilesystemLaunchInfo(argc, argv);
    if (!launchInfo) {
        SHOW_ERROR("Fatal: Could not resolve gameinfo.txt or -game path.");
        return EXIT_FAILURE;
    }

    const auto& fsPath  = launchInfo->fsDllPath;
    const auto& engPath = launchInfo->engineDllPath;
    std::string gameinfoStr = launchInfo->gameinfoPath.string();

    LOG("Launching with:\n  GameInfo: " + gameinfoStr +
        "\n  FS DLL:   " + fsPath +
        "\n  Engine:   " + engPath);

    RuntimeKernel kernel;
    if (!kernel.Initialize(fsPath, engPath, gameinfoStr)) {
        SHOW_ERROR("Fatal: RuntimeKernel failed to initialize.");
        return EXIT_FAILURE;
    }

    std::string gameName = launchInfo->gameinfoPath.parent_path().filename().string();
	std::string title = "INC [" + launchInfo->gameTitle + "]";
	int result = kernel.RunMainLoop(title.c_str());

    kernel.Shutdown();
    return result;
}