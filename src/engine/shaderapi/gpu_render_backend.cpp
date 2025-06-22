#include "shaderapi/gpu_render_backend.h"
#include "shaderapi/gpu_render_backend_gl.h"  // Current backend: OpenGL

// g_pGPURender: Global pointer to the active GPU rendering interface.
// Abstracts GPU operations like buffer uploads, shader binding, draw calls,
// and frame management across multiple graphics APIs (OpenGL, Vulkan, DirectX).
IGPURenderInterface* g_pGPURender = nullptr;

// GPURenderBackendGL - GPURenderBackendDX - GPURenderBackendVK

// Factory function to create the backend
IGPURenderInterface* CreateGPUAPI()
{
    if (!g_pGPURender)
    {
        // Choose backend — currently OpenGL only
        g_pGPURender = new GPURenderBackendGL();
    }
    return g_pGPURender;
}

// Clean up the current ShaderAPI instance
void DestroyGPUAPI()
{
    delete g_pGPURender;
    g_pGPURender = nullptr;
}