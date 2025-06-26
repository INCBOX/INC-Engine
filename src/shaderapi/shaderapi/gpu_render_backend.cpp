#include "shaderapi/gpu_render_backend.h"
#include "shaderapi/gpu_render_backend_gl.h"

// g_pGPURender: Global pointer to the active GPU rendering interface.
// Abstracts GPU operations like buffer uploads, shader binding, draw calls,
// and frame management across multiple graphics APIs (OpenGL, Vulkan, DirectX).
IGPURenderInterface* g_pGPURender = nullptr; // GLOBAL VARIABLE

// Internal creation logic — not exposed as extern "C"
static IGPURenderInterface* InternalCreateGPUAPI()
{
    if (!g_pGPURender)
    {
        // Choose backend — currently OpenGL only
        g_pGPURender = new GPURenderBackendGL();
    }
    return g_pGPURender;
}

// Internal cleanup logic — not exposed as extern "C"
static void InternalDestroyGPUAPI()
{
    delete g_pGPURender;
    g_pGPURender = nullptr;
}

// These are the actual exported functions with proper C linkage for the DLL
extern "C" __declspec(dllexport) IGPURenderInterface* CreateGPUAPI()
{
    return InternalCreateGPUAPI();
}

extern "C" __declspec(dllexport) void DestroyGPUAPI()
{
    InternalDestroyGPUAPI();
}