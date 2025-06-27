#include "engine_renderer.h"
#include "world/static_mesh_loader.h" // For GetStaticGeometry()
#include <iostream>

static IGPURenderInterface* s_pGPURender = nullptr;
static SDL_Window* s_Window = nullptr;

void Renderer_Init(IGPURenderInterface* gpuRender, SDL_Window* window) {
    s_pGPURender = gpuRender;
    s_Window = window;
}

void Renderer_RenderFrame(const Matrix4x4_f& viewMatrix, const Matrix4x4_f& projMatrix, float totalTime) {
    if (!s_pGPURender) return;

    int width, height;
    SDL_GetWindowSize(s_Window, &width, &height);

    s_pGPURender->BeginFrame();
    s_pGPURender->PrepareFrame(width, height);

    // Starfield rendering
    s_pGPURender->SetDepthMaskEnabled(false);
    s_pGPURender->SetDepthTestEnabled(false);
    s_pGPURender->RenderStarfield(totalTime);
    s_pGPURender->SetDepthMaskEnabled(true);
    s_pGPURender->SetDepthTestEnabled(true);

    s_pGPURender->SetViewMatrix(viewMatrix);
    s_pGPURender->SetProjectionMatrix(projMatrix);

    // Render static geometry
    const auto& staticGeometry = GetStaticGeometry();
    for (const auto& instance : staticGeometry) {
        s_pGPURender->DrawMesh(*instance.mesh, instance.transform);
    }

    s_pGPURender->EndFrame();
}

void Renderer_Shutdown() {
    if (s_pGPURender) {
        s_pGPURender->Shutdown();
        s_pGPURender = nullptr;
    }
    s_Window = nullptr;
}