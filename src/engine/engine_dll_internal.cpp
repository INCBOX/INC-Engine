// engine_dll.cpp — Wrapper/shim for exported engine API

#include "engine_dll_api.h"
#include "engine_dll_internal.h"
#include "engine_filesystem.h"
#include "engine_log.h"
#include "input.h"  // for g_Input

#include <SDL2/SDL.h>

#include <iostream>

extern Input g_Input;

extern "C" {
	
	DLL_EXPORT bool STDCALL Engine_Init(void* nativeWindowHandle) {
		return Engine_Internal_Init(nativeWindowHandle);
	}
	
	// Keep existing Engine_Init to just call Engine_Internal_Init as before (FILESYSTEM FOR)
    DLL_EXPORT bool STDCALL Engine_InitWithFS(void* fsDllHandle) {
        return LoadFileSystem(fsDllHandle);
    }
	
	DLL_EXPORT void STDCALL Engine_Shutdown() {
		Engine_Internal_Shutdown();
	}
	
	DLL_EXPORT bool STDCALL Engine_RunFrame(float deltaTime) {
		return Engine_Internal_RunFrame(deltaTime);
	}
	
	DLL_EXPORT void STDCALL Engine_ProcessSDLEvent(const SDL_Event* event) {
		g_Input.ProcessSDLEvent(*event);
	}
	
	// Optional: Self-contained run loop for standalone usage
	DLL_EXPORT void STDCALL Engine_Run() {
		Engine_Internal_Init(nullptr);
	
		Uint64 now = SDL_GetPerformanceCounter();
		Uint64 last = 0;
		double deltaTime = 0.0;
	
		while (true) {
			last = now;
			now = SDL_GetPerformanceCounter();
	
			deltaTime = (double)((now - last) * 1000 / (double)SDL_GetPerformanceFrequency());
	
			bool keepRunning = Engine_Internal_RunFrame(static_cast<float>(deltaTime / 1000.0f));
			if (!keepRunning)
				break;
		}
	
		Engine_Internal_Shutdown();
	}

} // extern "C"
