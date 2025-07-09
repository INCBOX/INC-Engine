// what is this runtime kernel? It’s the central runtime control layer for your launcher (inc.exe) that owns and orchestrates all loaded modules, manages the main loop, and can be extended later for:

// Hot module reload
// Dynamic module loading/unloading on demand
// Dependency tracking between modules
// Messaging and events between modules

// WILL BE ADDED
// Hot reload support (unloading/reloading DLLs on the fly)
// More sophisticated dependency or messaging system between modules
// Separate modules for input, rendering, gameplay, etc. (for truly modular runtime)

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include "runtime_kernel.h"

#include "dll_loader.h"

#include <iostream>

RuntimeKernel::RuntimeKernel() = default;
RuntimeKernel::~RuntimeKernel() { Shutdown(); }

bool RuntimeKernel::Initialize(const std::string& fsDllPath, const std::string& engineDllPath, const std::string& gameinfoPath) {
    if (!LoadFilesystem(fsDllPath, gameinfoPath)) {
        std::cerr << "[RuntimeKernel] Failed to load filesystem DLL\n";
        return false;
    }

    if (!LoadEngine(engineDllPath)) {
        std::cerr << "[RuntimeKernel] Failed to load engine DLL\n";
        return false;
    }

    return true;
}

bool RuntimeKernel::LoadFilesystem(const std::string& path, const std::string& gameinfoPath) {
    fsLib = LoadLib(path.c_str());
    if (!fsLib) {
        std::cerr << "[RuntimeKernel] Failed to load filesystem DLL: " << path << "\n";
        return false;
    }

    FS_Init = reinterpret_cast<FSInitFn>(GetLibProc(fsLib, "FS_Init"));
    if (!FS_Init) {
        std::cerr << "[RuntimeKernel] FS_Init symbol not found\n";
        CloseLib(fsLib);
        fsLib = nullptr;
        return false;
    }

    if (!FS_Init(gameinfoPath)) {
        std::cerr << "[RuntimeKernel] FS_Init failed during initialization\n";
        CloseLib(fsLib);
        fsLib = nullptr;
        return false;
    }

    return true;
}

bool RuntimeKernel::LoadEngine(const std::string& path) {
    engineLib = LoadLib(path.c_str());
    if (!engineLib) {
        std::cerr << "[RuntimeKernel] Failed to load engine DLL: " << path << "\n";
        return false;
    }

    Engine_InitWithFS_ = reinterpret_cast<EngineInitWithFSFn>(GetLibProc(engineLib, "Engine_InitWithFS"));
    Engine_Init = reinterpret_cast<EngineInitFnNew>(GetLibProc(engineLib, "Engine_Init"));
    Engine_RunFrame = reinterpret_cast<EngineRunFrameFn>(GetLibProc(engineLib, "Engine_RunFrame"));
    Engine_Shutdown = reinterpret_cast<EngineShutdownFn>(GetLibProc(engineLib, "Engine_Shutdown"));
    Engine_ProcessSDLEvent = reinterpret_cast<EngineProcessSDLEventFn>(GetLibProc(engineLib, "Engine_ProcessSDLEvent"));

    if (!Engine_InitWithFS_ || !Engine_Init || !Engine_RunFrame || !Engine_Shutdown || !Engine_ProcessSDLEvent) {
        std::cerr << "[RuntimeKernel] Failed to get all engine function pointers\n";
        CloseLib(engineLib);
        engineLib = nullptr;
        return false;
    }

    if (!Engine_InitWithFS_(static_cast<void*>(fsLib))) {
        std::cerr << "[RuntimeKernel] Engine_InitWithFS failed\n";
        CloseLib(engineLib);
        engineLib = nullptr;
        return false;
    }

    return true;
}

int RuntimeKernel::RunMainLoop(const std::string& windowTitle) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        std::cerr << "[RuntimeKernel] SDL_Init failed: " << SDL_GetError() << "\n";
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow(
        windowTitle.c_str(),
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1280, 720,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    if (!window) {
        std::cerr << "[RuntimeKernel] Failed to create SDL window: " << SDL_GetError() << "\n";
        SDL_Quit();
        return -2;
    }

    if (!Engine_Init(static_cast<void*>(window))) {
        std::cerr << "[RuntimeKernel] Engine_Init (window) failed\n";
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -3;
    }

    Uint64 last = SDL_GetPerformanceCounter();
    Uint64 now = last;
    double deltaTime = 0.0;

    while (true) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                std::cout << "[RuntimeKernel] SDL_QUIT received, exiting.\n";
                goto cleanup;
            }
            Engine_ProcessSDLEvent(&event);
        }

        now = SDL_GetPerformanceCounter();
        deltaTime = (now - last) / (double)SDL_GetPerformanceFrequency();

        if (!Engine_RunFrame(static_cast<float>(deltaTime)))
            break;

        last = now;
    }

cleanup:
    Engine_Shutdown();
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

void RuntimeKernel::Shutdown() {
    if (engineLib) {
        CloseLib(engineLib);
        engineLib = nullptr;
    }
    if (fsLib) {
        CloseLib(fsLib);
        fsLib = nullptr;
    }
}