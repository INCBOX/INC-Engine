#include "engine_renderer.h"
#include "world/static_mesh_loader.h" // For GetStaticGeometry()
#include <iostream>
#include <Windows.h>

static HMODULE g_ShaderAPIDLL = nullptr;
typedef IGPURenderInterface* (*CreateGPUAPI_t)();
typedef void (*DestroyGPUAPI_t)();

static CreateGPUAPI_t pCreateGPUAPI = nullptr;
static DestroyGPUAPI_t pDestroyGPUAPI = nullptr;


static IGPURenderInterface* s_pGPURender = nullptr;
static SDL_Window* s_Window = nullptr;

void Renderer_Init(IGPURenderInterface* gpuRender, SDL_Window* window) {
    s_pGPURender = gpuRender;
    s_Window = window;
}

bool Renderer_LoadAndInit(SDL_Window* window) {
    g_ShaderAPIDLL = LoadLibraryA("bin/shaderapi.dll");
    if (!g_ShaderAPIDLL) {
        std::cerr << "[Renderer] Failed to load shaderapi.dll\n";
        return false;
    }

    pCreateGPUAPI = (CreateGPUAPI_t)GetProcAddress(g_ShaderAPIDLL, "CreateGPUAPI");
    pDestroyGPUAPI = (DestroyGPUAPI_t)GetProcAddress(g_ShaderAPIDLL, "DestroyGPUAPI");

    if (!pCreateGPUAPI || !pDestroyGPUAPI) {
        std::cerr << "[Renderer] Failed to get shaderapi exports\n";
        FreeLibrary(g_ShaderAPIDLL);
        g_ShaderAPIDLL = nullptr;
        return false;
    }

    s_pGPURender = pCreateGPUAPI();
    if (!s_pGPURender || !s_pGPURender->Init(window, 1280, 720)) {
        std::cerr << "[Renderer] Failed to initialize GPU backend!\n";
        if (s_pGPURender) {
            s_pGPURender->Shutdown();
            s_pGPURender = nullptr;
        }
        FreeLibrary(g_ShaderAPIDLL);
        g_ShaderAPIDLL = nullptr;
        return false;
    }

    Renderer_Init(s_pGPURender, window);
    return true;
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

void Renderer_Unload() {
    Renderer_Shutdown();

    if (g_ShaderAPIDLL) {
        FreeLibrary(g_ShaderAPIDLL);
        g_ShaderAPIDLL = nullptr;
    }

    pCreateGPUAPI = nullptr;
    pDestroyGPUAPI = nullptr;
}


IGPURenderInterface* GetRenderInterface() {
    return s_pGPURender;
}