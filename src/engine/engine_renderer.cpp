#include "engine_renderer.h"
#include "world/static_mesh_loader.h" // For GetStaticGeometry()
#include <iostream>
#include <SDL2/SDL.h>

//-----------------------------------------------------------------------------
// Renderer state
//-----------------------------------------------------------------------------
static IGPURenderInterface* s_pGPURender = nullptr;
static SDL_Window* s_Window = nullptr;

//-----------------------------------------------------------------------------
// Initialize renderer state with GPU interface and window (no Init call!)
//-----------------------------------------------------------------------------
bool Renderer_Init(IGPURenderInterface* gpuRender, SDL_Window* window) {
    if (!gpuRender || !window)
        return false;

    s_pGPURender = gpuRender;
    s_Window = window;

    return true;
}

//-----------------------------------------------------------------------------
// Per-frame rendering logic
//-----------------------------------------------------------------------------
void Renderer_RenderFrame(const Matrix4x4_f& viewMatrix, const Matrix4x4_f& projMatrix, float totalTime) {
    if (!s_pGPURender || !s_Window)
        return;

    int width, height;
    SDL_GetWindowSize(s_Window, &width, &height);

    s_pGPURender->BeginFrame();

    s_pGPURender->PrepareFrame(width, height);

    s_pGPURender->SetDepthMaskEnabled(false);
    s_pGPURender->SetDepthTestEnabled(false);
    s_pGPURender->RenderStarfield(totalTime);
    s_pGPURender->SetDepthMaskEnabled(true);
    s_pGPURender->SetDepthTestEnabled(true);

    s_pGPURender->SetViewMatrix(viewMatrix);
    s_pGPURender->SetProjectionMatrix(projMatrix);

    const auto& staticGeometry = GetStaticGeometry();
    for (const auto& instance : staticGeometry) {
        s_pGPURender->DrawMesh(*instance.mesh, instance.transform);
    }

    s_pGPURender->EndFrame();
}

//-----------------------------------------------------------------------------
// Shutdown GPU interface and cleanup
//-----------------------------------------------------------------------------
void Renderer_Shutdown() {
    if (s_pGPURender) {
        s_pGPURender->Shutdown();
        s_pGPURender = nullptr;
    }
    s_Window = nullptr;
}

//-----------------------------------------------------------------------------
// Accessor for global GPU render interface pointer
//-----------------------------------------------------------------------------
IGPURenderInterface* GetRenderInterface() {
    return s_pGPURender;
}