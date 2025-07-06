#include <windows.h>
#include <cstdio>
#include <cstdlib>
#include "shaderapi/gpu_render_backend.h"

using CreateGPUAPI_t = IGPURenderInterface* (*)();
using DestroyGPUAPI_t = void (*)();

static HMODULE s_hBackendLib = nullptr;
static CreateGPUAPI_t s_pfnCreate = nullptr;
static DestroyGPUAPI_t s_pfnDestroy = nullptr;

IGPURenderInterface* InternalCreateGPUAPI()
{
    if (g_pGPURender)
        return g_pGPURender;

    char basePath[MAX_PATH];
    if (!GetModuleFileNameA(GetModuleHandleA("shaderapi.dll"), basePath, MAX_PATH))
        return nullptr;

    // Trim to directory
    for (int i = (int)strlen(basePath) - 1; i >= 0; --i) {
        if (basePath[i] == '\\' || basePath[i] == '/') {
            basePath[i + 1] = '\0';
            break;
        }
    }

    const char* overrideName = getenv("INC_GPU_BACKEND");
	const char* defaults[] = { "shaderapi_null.dll", "shaderapi_gl.dll", "shaderapi_dx.dll", "shaderapi_vk.dll" };

    auto tryLoad = [&](const char* name) -> bool {
        char path[MAX_PATH];
        snprintf(path, sizeof(path), "%s%s", basePath, name);

        HMODULE lib = LoadLibraryA(path);
        if (!lib) return false;

        auto create = (CreateGPUAPI_t)GetProcAddress(lib, "CreateGPUAPI");
        auto destroy = (DestroyGPUAPI_t)GetProcAddress(lib, "DestroyGPUAPI");

        if (create && destroy) {
            s_hBackendLib = lib;
            s_pfnCreate = create;
            s_pfnDestroy = destroy;
            g_pGPURender = s_pfnCreate();
            return true;
        }

        FreeLibrary(lib);
        return false;
    };

    if (overrideName && tryLoad(overrideName))
        return g_pGPURender;

    for (const char* backend : defaults)
        if (tryLoad(backend))
            return g_pGPURender;

    MessageBoxA(nullptr,
        "No compatible GPU backend could be loaded.\n"
        "Expected one of:\n"
        "  shaderapi_vk.dll\n"
        "  shaderapi_gl.dll\n"
        "  shaderapi_dx.dll\n"
        "in the bin/ directory next to shaderapi.dll.",
        "ShaderAPI Error", MB_ICONERROR);

    return nullptr;
}

void InternalDestroyGPUAPI()
{
    if (s_pfnDestroy && g_pGPURender)
        s_pfnDestroy();

    g_pGPURender = nullptr;

    if (s_hBackendLib) {
        FreeLibrary(s_hBackendLib);
        s_hBackendLib = nullptr;
    }

    s_pfnCreate = nullptr;
    s_pfnDestroy = nullptr;
}

extern "C" {

__declspec(dllexport) IGPURenderInterface* CreateGPUAPI()
{
    return InternalCreateGPUAPI();
}

__declspec(dllexport) void DestroyGPUAPI()
{
    InternalDestroyGPUAPI();
}

__declspec(dllexport) IGPURenderInterface* g_pGPURender = nullptr;

}
