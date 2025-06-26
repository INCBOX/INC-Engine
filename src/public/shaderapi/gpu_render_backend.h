// Entrypoint header: Public-facing API — defines GPU interface for external engine

#pragma once
#include "shaderapi/gpu_render_interface.h"

#ifdef BUILDING_SHADERAPI_DLL

extern "C" {

__declspec(dllexport) IGPURenderInterface* CreateGPUAPI();
__declspec(dllexport) void DestroyGPUAPI();
__declspec(dllexport) extern IGPURenderInterface* g_pGPURender;

}

#else

// When used by engine, don't link these — declare them dynamically
// The engine should resolve them manually with GetProcAddress()
// g_pGPURender is defined in engine's static scope, not here

#endif