#include "shaderapi/gpu_render_backend_gl.h"

static GPURenderBackendGL* s_pBackendGL = nullptr;

extern "C" {

// Exported factory function to create the GL renderer instance
__declspec(dllexport) IGPURenderInterface* CreateGPUAPI()
{
    if (!s_pBackendGL) {
        s_pBackendGL = new GPURenderBackendGL();
    }
    return s_pBackendGL;
}

// Exported function to destroy the GL renderer instance
__declspec(dllexport) void DestroyGPUAPI(IGPURenderInterface* pRender)
{
    if (pRender == s_pBackendGL && s_pBackendGL) {
        s_pBackendGL->Shutdown();
        delete s_pBackendGL;
        s_pBackendGL = nullptr;
    }
}

}