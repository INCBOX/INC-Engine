//
// platform_host.cpp — Platform Host Main Loop and DLL Loader
//

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include "engine_dll_api.h"  // FOR INPUT STUFF

#include <iostream>
#include <string>

#if defined(_WIN32)
    #include <Windows.h>
    using LibHandle = HMODULE;
    #define LoadLib(path) LoadLibraryExA(path, NULL, LOAD_WITH_ALTERED_SEARCH_PATH)
    #define GetLibProc(lib, name) GetProcAddress(lib, name)
    #define CloseLib(lib) FreeLibrary(lib)
#else
    #include <dlfcn.h>
    using LibHandle = void*;
    inline LibHandle LoadLib(const char* path) { return dlopen(path, RTLD_NOW); }
    inline void* GetLibProc(LibHandle lib, const char* name) { return dlsym(lib, name); }
    inline void CloseLib(LibHandle lib) { if (lib) dlclose(lib); }
#endif

// Function pointer types
using FSInitFn = bool(*)(const std::string&);
using EngineInitFnNew = bool(*)(void*);
using EngineInitWithFSFn = bool(*)(void*);
using EngineRunFrameFn = bool(*)(float);
using EngineShutdownFn = void(*)();
using EngineProcessSDLEventFn = void(*)(const SDL_Event*);

EngineProcessSDLEventFn Engine_ProcessSDLEvent = nullptr;

int PlatformHost_Run(
    const std::string& engineDllPath,
    const std::string& fsDllPath,
    const std::string& gameInfoPath,
    const std::string& windowTitle)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        std::cerr << "[PlatformHost] SDL_Init failed: " << SDL_GetError() << "\n";
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow(
        windowTitle.c_str(),
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1280, 720,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    if (!window) {
        std::cerr << "[PlatformHost] Failed to create SDL window: " << SDL_GetError() << "\n";
        SDL_Quit();
        return -2;
    }

    std::cout << "[PlatformHost] Loading filesystem DLL: " << fsDllPath << "\n";
    LibHandle fsLib = LoadLib(fsDllPath.c_str());
    if (!fsLib) {
        std::cerr << "[PlatformHost] Failed to load filesystem DLL: " << fsDllPath << "\n";
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -3;
    }

    auto FS_Init = reinterpret_cast<FSInitFn>(GetLibProc(fsLib, "FS_Init"));
    if (!FS_Init || !FS_Init(gameInfoPath)) {
        std::cerr << "[PlatformHost] Filesystem initialization failed\n";
        CloseLib(fsLib);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -4;
    }

    std::cout << "[PlatformHost] Loading engine DLL: " << engineDllPath << "\n";
    LibHandle engineLib = LoadLib(engineDllPath.c_str());
    if (!engineLib) {
        std::cerr << "[PlatformHost] Failed to load engine DLL: " << engineDllPath << "\n";
        CloseLib(fsLib);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -5;
    }

    EngineInitWithFSFn pEngine_InitWithFS = reinterpret_cast<EngineInitWithFSFn>(GetLibProc(engineLib, "Engine_InitWithFS"));
    EngineInitFnNew pEngine_Init = reinterpret_cast<EngineInitFnNew>(GetLibProc(engineLib, "Engine_Init"));
    EngineRunFrameFn pEngine_RunFrame = reinterpret_cast<EngineRunFrameFn>(GetLibProc(engineLib, "Engine_RunFrame"));
    EngineShutdownFn pEngine_Shutdown = reinterpret_cast<EngineShutdownFn>(GetLibProc(engineLib, "Engine_Shutdown"));
    EngineProcessSDLEventFn pEngine_ProcessSDLEvent = reinterpret_cast<EngineProcessSDLEventFn>(GetLibProc(engineLib, "Engine_ProcessSDLEvent"));

    if (!pEngine_InitWithFS || !pEngine_Init || !pEngine_RunFrame || !pEngine_Shutdown || !pEngine_ProcessSDLEvent) {
        std::cerr << "[PlatformHost] Failed to get engine function pointers\n";
        CloseLib(engineLib);
        CloseLib(fsLib);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -6;
    }

    // Initialize engine with filesystem DLL handle first
    if (!pEngine_InitWithFS(static_cast<void*>(fsLib))) {
        std::cerr << "[PlatformHost] Engine_InitWithFS failed\n";
        CloseLib(engineLib);
        CloseLib(fsLib);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -7;
    }

    // Initialize engine with SDL_Window* handle second
    if (!pEngine_Init(static_cast<void*>(window))) {
        std::cerr << "[PlatformHost] Engine_Init (window) failed\n";
        CloseLib(engineLib);
        CloseLib(fsLib);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -8;
    }

    Uint64 now = SDL_GetPerformanceCounter();
    Uint64 last = 0;
    double deltaTime = 0.0;
    SDL_Event event;

    while (true) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                std::cout << "[PlatformHost] SDL_QUIT received — exiting main loop.\n";
                goto cleanup;
            }

            if (pEngine_ProcessSDLEvent)
                pEngine_ProcessSDLEvent(&event);
        }

        last = now;
        now = SDL_GetPerformanceCounter();
        deltaTime = (double)((now - last) * 1000 / (double)SDL_GetPerformanceFrequency());

        if (!pEngine_RunFrame(static_cast<float>(deltaTime / 1000.0f)))
            break;
    }

cleanup:
    pEngine_Shutdown();
    SDL_DestroyWindow(window);
    SDL_Quit();
    CloseLib(engineLib);
    CloseLib(fsLib);

    return 0;
}