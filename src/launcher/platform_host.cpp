#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include <string>
#include <iostream>
#include "inc_dll_utils.h" // for LoadLib, GetLibProc, CloseLib, LibHandle

// Optional error message helper
inline void ShowError(const std::string& msg) {
    std::cerr << "[PlatformHost Error] " << msg << "\n";
}

// Typedefs for engine/filesystem functions
typedef bool (*EngineInitWithFSFn)(void* fsLibHandle);
typedef bool (*EngineInitFnNew)(void* sdlWindow);
typedef bool (*EngineRunFrameFn)(float dt);
typedef void (*EngineShutdownFn)();
typedef void (*EngineProcessSDLEventFn)(SDL_Event*);

// Main launcher function
int PlatformHost_Run(
    const std::string& engineDllPath,
    LibHandle fsLib,
    const std::string& gameInfoPath,
    const std::string& windowTitle)
{
    if (!fsLib) {
        ShowError("Filesystem DLL handle invalid");
        return -2;
    }

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

    LibHandle engineLib = LoadLib(engineDllPath.c_str());
    if (!engineLib) {
        std::cerr << "[PlatformHost] Failed to load engine DLL: " << engineDllPath << "\n";
        CloseLib(fsLib);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -5;
    }

    auto pEngine_InitWithFS = reinterpret_cast<EngineInitWithFSFn>(GetLibProc(engineLib, "Engine_InitWithFS"));
    auto pEngine_Init = reinterpret_cast<EngineInitFnNew>(GetLibProc(engineLib, "Engine_Init"));
    auto pEngine_RunFrame = reinterpret_cast<EngineRunFrameFn>(GetLibProc(engineLib, "Engine_RunFrame"));
    auto pEngine_Shutdown = reinterpret_cast<EngineShutdownFn>(GetLibProc(engineLib, "Engine_Shutdown"));
    auto pEngine_ProcessSDLEvent = reinterpret_cast<EngineProcessSDLEventFn>(GetLibProc(engineLib, "Engine_ProcessSDLEvent"));

    if (!pEngine_InitWithFS || !pEngine_Init || !pEngine_RunFrame || !pEngine_Shutdown || !pEngine_ProcessSDLEvent) {
        std::cerr << "[PlatformHost] Failed to get engine function pointers\n";
        CloseLib(engineLib);
        CloseLib(fsLib);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -6;
    }

    if (!pEngine_InitWithFS(static_cast<void*>(fsLib))) {
        std::cerr << "[PlatformHost] Engine_InitWithFS failed\n";
        CloseLib(engineLib);
        CloseLib(fsLib);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -7;
    }

    if (!pEngine_Init(static_cast<void*>(window))) {
        std::cerr << "[PlatformHost] Engine_Init (window) failed\n";
        CloseLib(engineLib);
        CloseLib(fsLib);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -8;
    }

	Uint64 last = SDL_GetPerformanceCounter();
	Uint64 now = last;
	double deltaTime = 0.0;
	
	while (true) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				std::cout << "[PlatformHost] SDL_QUIT received — exiting main loop.\n";
				goto cleanup;
			}
			pEngine_ProcessSDLEvent(&event);
		}
	
		now = SDL_GetPerformanceCounter();
		deltaTime = (now - last) / (double)SDL_GetPerformanceFrequency();
	
		if (!pEngine_RunFrame(static_cast<float>(deltaTime)))
			break;
	
		last = now;
	}

cleanup:
    pEngine_Shutdown();
    SDL_DestroyWindow(window);
    SDL_Quit();
    CloseLib(engineLib);
    CloseLib(fsLib);

    return 0;
}