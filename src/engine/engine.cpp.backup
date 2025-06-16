// engine.cpp
// Core engine logic managing SDL window, ShaderAPI abstraction, filesystem DLL loading,
// map loading, and main loop. Uses fully modular ShaderAPI backend (OpenGL, Vulkan, etc).

#include <Windows.h>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include "nlohmann/json.hpp"

#include "engine_api.h"
#include "shaderapi/shaderapi.h" // Modular ShaderAPI interface (replaces shaderapi_gl.h)

#include "world/geometry_loader.h" // FOR JSON LOAD GEOMETRY

using json = nlohmann::json;

//-----------------------------------------------------------------------------
// FileSystem DLL dynamic loading
//-----------------------------------------------------------------------------
static HMODULE g_FileSystemDLL = nullptr;

typedef const std::string& (*FS_GetGameDirFn)();
typedef std::string (*FS_ResolvePathFn)(const std::string&);
static FS_GetGameDirFn FS_GetGameDir = nullptr;
static FS_ResolvePathFn FS_ResolvePath = nullptr;

//-----------------------------------------------------------------------------
// SDL + Renderer
//-----------------------------------------------------------------------------
static SDL_Window* g_Window = nullptr;
static ShaderAPICore* g_Renderer = nullptr;

//-----------------------------------------------------------------------------
// Cross-platform export macros
//-----------------------------------------------------------------------------
#if defined(_WIN32)
#define DLL_EXPORT extern "C" __declspec(dllexport)
#define STDCALL __stdcall
#else
#define DLL_EXPORT extern "C" __attribute__((visibility("default")))
#define STDCALL
#endif

//-----------------------------------------------------------------------------
// Logging
//-----------------------------------------------------------------------------
void InitEngineLog() {
    std::filesystem::create_directories("logs");
    FILE* logFile;
    freopen_s(&logFile, "logs/engine.log", "w", stdout);
    freopen_s(&logFile, "logs/engine.log", "w", stderr);
    setvbuf(stdout, nullptr, _IONBF, 0);
    setvbuf(stderr, nullptr, _IONBF, 0);
    std::cout << "[Engine] Log started\n";
}

//-----------------------------------------------------------------------------
// FileSystem loader
//-----------------------------------------------------------------------------
bool LoadFileSystem() {
    g_FileSystemDLL = LoadLibraryExA("bin/filesystem_stdio.dll", NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
    if (!g_FileSystemDLL) {
        std::cerr << "[Engine] Failed to load filesystem_stdio.dll\n";
        return false;
    }

    FS_GetGameDir = (FS_GetGameDirFn)GetProcAddress(g_FileSystemDLL, "FS_GetGameDir");
    FS_ResolvePath = (FS_ResolvePathFn)GetProcAddress(g_FileSystemDLL, "FS_ResolvePath");

    if (!FS_GetGameDir || !FS_ResolvePath) {
        std::cerr << "[Engine] Failed to resolve FileSystem exports\n";
        return false;
    }

    std::cout << "[Engine] FileSystem loaded\n";
    return true;
}

//-----------------------------------------------------------------------------
// Map loader (JSON-based for now)
//-----------------------------------------------------------------------------
bool LoadMap(const std::string& mapName) {
    std::string relative = "maps/" + mapName + ".json";
    std::string resolved = FS_ResolvePath(relative);

    if (resolved.empty()) {
        std::cerr << "[Engine] Map not found: " << relative << "\n";
        return false;
    }

    std::ifstream mapFile(resolved);
    if (!mapFile.is_open()) {
        std::cerr << "[Engine] Failed to open map: " << resolved << "\n";
        return false;
    }

    json mapData;
    try {
        mapFile >> mapData;
    } catch (const std::exception& e) {
        std::cerr << "[Engine] JSON parsing error: " << e.what() << "\n";
        return false;
    }

    if (mapData.contains("entities") && mapData["entities"].is_array()) {
        for (const auto& entity : mapData["entities"]) {
            std::string classname = entity.value("classname", "unknown");
            auto origin = entity.value("origin", std::vector<float>{0, 0, 0});
            std::cout << "[Entity] " << classname << " at ("
                      << origin[0] << ", " << origin[1] << ", " << origin[2] << ")\n";
        }
    }

    LoadStaticGeometryFromMap(mapData);

    return true;
}

//-----------------------------------------------------------------------------
// Engine Initialization
//-----------------------------------------------------------------------------
DLL_EXPORT void STDCALL Engine_Init() {
    SDL_SetMainReady();

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        std::cerr << "[Engine] SDL_Init failed: " << SDL_GetError() << "\n";
        return;
    }

    g_Window = SDL_CreateWindow("INC Engine",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (!g_Window) {
        std::cerr << "[Engine] SDL_CreateWindow failed: " << SDL_GetError() << "\n";
        return;
    }

    g_Renderer = CreateShaderAPI(); // Modular backend creation
    if (!g_Renderer || !g_Renderer->Init(g_Window, 1280, 720)) {
        std::cerr << "[Engine] ShaderAPI Init failed\n";
        return;
    }

    std::cout << "[Engine] SDL + ShaderAPI initialized\n";
}

//-----------------------------------------------------------------------------
// Engine Frame
//-----------------------------------------------------------------------------
DLL_EXPORT bool STDCALL Engine_RunFrame() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            std::cout << "[Engine] SDL_QUIT event received\n";
            return false;
        }
    }

    int width, height;
    SDL_GetWindowSize(g_Window, &width, &height);

    if (!g_Renderer) {
        std::cerr << "[Engine] Renderer is null!\n";
        return false;
    }

    g_Renderer->BeginFrame();
    g_Renderer->PrepareFrame(width, height);
	
    // Render static geometry from loaded map
    for (const auto& instance : GetStaticGeometry()) {
        g_Renderer->DrawMesh(*instance.mesh, instance.transform);
    }

    g_Renderer->EndFrame();

    return true;
}

//-----------------------------------------------------------------------------
// Engine Shutdown
//-----------------------------------------------------------------------------
DLL_EXPORT void STDCALL Engine_Shutdown() {
    if (g_Renderer) {
        g_Renderer->Shutdown();
        DestroyShaderAPI(); // ✅ Proper deletion
        g_Renderer = nullptr;
    }

    if (g_Window) {
        SDL_DestroyWindow(g_Window);
        g_Window = nullptr;
    }

    SDL_Quit();

    if (g_FileSystemDLL) {
        FreeLibrary(g_FileSystemDLL);
        g_FileSystemDLL = nullptr;
    }

    std::cout << "[Engine] Shutdown complete\n";
}

//-----------------------------------------------------------------------------
// Engine Main Entry Point
//-----------------------------------------------------------------------------
DLL_EXPORT void STDCALL Engine_Run() {
    InitEngineLog();
    std::cout << "[Engine] Starting Engine_Run\n";

    Engine_Init();

    if (!LoadFileSystem()) {
        std::cerr << "[Engine] Failed to load filesystem\n";
        return;
    }

    if (!LoadMap("start")) {
        std::cerr << "[Engine] Failed to load start map\n";
        return;
    }

    std::cout << "[Engine] Entering main loop\n";

    while (true) {
        bool keepRunning = Engine_RunFrame();
        if (!keepRunning) {
            std::cout << "[Engine] Engine_RunFrame returned false, exiting loop\n";
            break;
        }
        SDL_Delay(16); // ~60 FPS
    }

    Engine_Shutdown();
    std::cout << "[Engine] Engine_Run exiting\n";
}