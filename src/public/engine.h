#pragma once

#if defined(_WIN32)
#define DLL_EXPORT extern "C" __declspec(dllexport)
#define STDCALL __stdcall
#else
#define DLL_EXPORT extern "C" __attribute__((visibility("default")))
#define STDCALL
#endif

DLL_EXPORT void STDCALL Engine_Run();
DLL_EXPORT void STDCALL Engine_Shutdown();
DLL_EXPORT bool STDCALL Engine_RunFrame(); // <- return bool now