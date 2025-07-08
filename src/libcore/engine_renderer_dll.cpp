#include "engine_renderer_dll.h"
#include "engine_renderer.h"
#include <iostream>
#include <filesystem>
#include <SDL2/SDL.h>
#include <Windows.h>

static HMODULE g_ShaderAPIDLL = nullptr;
static IGPURenderInterface* g_GPU = nullptr;

using CreateGPUAPI_t = IGPURenderInterface* (*)();
using DestroyGPUAPI_t = void (*)(IGPURenderInterface*);
static CreateGPUAPI_t CreateGPUAPI = nullptr;
static DestroyGPUAPI_t DestroyGPUAPI = nullptr;

static void Cleanup() {
    if (g_GPU) {
        g_GPU->Shutdown();
        if (DestroyGPUAPI) DestroyGPUAPI(g_GPU);
        g_GPU = nullptr;
    }
    if (g_ShaderAPIDLL) {
        FreeLibrary(g_ShaderAPIDLL);
        g_ShaderAPIDLL = nullptr;
    }
    CreateGPUAPI = nullptr;
    DestroyGPUAPI = nullptr;
    Renderer_Init(nullptr, nullptr);
}

bool RendererDLL_LoadAndInit(SDL_Window* window) {
    char exePath[MAX_PATH];
    if (!GetModuleFileNameA(nullptr, exePath, MAX_PATH)) return false;
    auto dllPath = std::filesystem::path(exePath).parent_path() / "bin" / "shaderapi.dll";

    g_ShaderAPIDLL = LoadLibraryA(dllPath.string().c_str());
    if (!g_ShaderAPIDLL) {
        std::cerr << "[RendererDLL] Failed to load shaderapi.dll\n";
        return false;
    }

    CreateGPUAPI = (CreateGPUAPI_t)GetProcAddress(g_ShaderAPIDLL, "CreateGPUAPI");
    DestroyGPUAPI = (DestroyGPUAPI_t)GetProcAddress(g_ShaderAPIDLL, "DestroyGPUAPI");
    if (!CreateGPUAPI || !DestroyGPUAPI) {
        std::cerr << "[RendererDLL] Missing exports in shaderapi.dll\n";
        Cleanup();
        return false;
    }

    g_GPU = CreateGPUAPI();
    if (!g_GPU) {
        std::cerr << "[RendererDLL] Failed to create GPU interface\n";
        Cleanup();
        return false;
    }

    int w = 0, h = 0;
    SDL_GetWindowSize(window, &w, &h);
    if (!g_GPU->Init(window, w, h)) {
        std::cerr << "[RendererDLL] GPU Init failed\n";
        Cleanup();
        return false;
    }

    Renderer_Init(g_GPU, window);
    return true;
}

void RendererDLL_Unload() {
    Cleanup();
}