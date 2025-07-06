// public DLL API header- public, stable DLL interface, never include engine internals

#pragma once

//-----------------------------------------------------------------------------
// Cross-platform export macros
//-----------------------------------------------------------------------------
#if defined(_WIN32)
#define DLL_EXPORT __declspec(dllexport)
#define STDCALL __stdcall
#else
#define DLL_EXPORT __attribute__((visibility("default")))
#define STDCALL
#endif

#ifdef __cplusplus
extern "C" {
#endif

// Pass native window handle from launcher to engine init
DLL_EXPORT bool STDCALL Engine_Init(void* nativeWindowHandle);

// DLL_EXPORT void STDCALL Engine_Run();         // Defined in engine.cpp only

// Shutdown engine and cleanup
DLL_EXPORT void STDCALL Engine_Shutdown();

// Engine_RunFrame should be called AFTER events are polled in launcher
// Engine no longer handles SDL events internally!
DLL_EXPORT bool STDCALL Engine_RunFrame(float deltaTime);

#ifdef __cplusplus
}
#endif