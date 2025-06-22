//-----------------------------------------------------------------------------
// ENGINE.CPP - INC. INC© INCBOX 2007 ALL RIGHTS RESERVED.
// Core engine logic: SDL window, ShaderAPI abstraction, filesystem DLL loading,
// map loading (JSON), input, player, and main loop.
//-----------------------------------------------------------------------------

#include <Windows.h>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include "nlohmann/json.hpp"

#include "engine_api.h"
#include "engine_globals.h"				// access the main camera from anywhere in engine
#include "engine_log.h"
#include "shaderapi/shaderapi.h" 		// Modular ShaderAPI interface

#include "world/static_mesh_loader.h" 		// Static geometry loader (JSON)

#include "input.h"
#include "mathlib/camera_f.h"
#include "player.h"

using json = nlohmann::json;

//-----------------------------------------------------------------------------
// DEBUG -O3 -DNDEBUG -march=native,
//-----------------------------------------------------------------------------
#ifdef DEBUG
#define LOG(x) std::cout << x << std::endl;
#else
#define LOG(x)
#endif

//-----------------------------------------------------------------------------
// Globals
//-----------------------------------------------------------------------------
static Camera g_Camera;
static Input g_Input;
static Player g_Player;

//-----------------------------------------------------------------------------
// FileSystem DLL dynamic loading
//-----------------------------------------------------------------------------
static HMODULE g_FileSystemDLL = nullptr;

typedef const std::string& (*FS_GetGameDirFn)();
typedef std::string (*FS_ResolvePathFn)(const std::string&);
static FS_GetGameDirFn FS_GetGameDir = nullptr;
static FS_ResolvePathFn FS_ResolvePath = nullptr;

//-----------------------------------------------------------------------------
// SDL + ShaderAPI
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
// Logging setup
//-----------------------------------------------------------------------------
void InitEngineLog() {
    EngineLog_Init();
    EngineLog("[Engine] Log started");
}

//-----------------------------------------------------------------------------
// Load filesystem DLL and get function pointers
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
        FreeLibrary(g_FileSystemDLL);
        g_FileSystemDLL = nullptr;
        return false;
    }

    std::cout << "[Engine] FileSystem loaded\n";
    return true;
}

//-----------------------------------------------------------------------------
// Load JSON map, parse entities, and load static geometry
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
// Initialize SDL, window, input, and ShaderAPI
//-----------------------------------------------------------------------------
DLL_EXPORT void STDCALL Engine_Init() {
    SDL_SetMainReady();

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        std::cerr << "[Engine] SDL_Init failed: " << SDL_GetError() << "\n";
        return;
    }

    g_Window = SDL_CreateWindow(
        "INC Engine",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1280, 720,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
    );
	
    if (!g_Window) {
        std::cerr << "[Engine] SDL_CreateWindow failed: " << SDL_GetError() << "\n";
        SDL_Quit();
        return;
    }

    // Enable VSync for stable FPS and reduce tearing
    // SDL_GL_SetSwapInterval(1); VSync

    // Enable relative mouse mode: grab and hide cursor for FPS controls
	SDL_SetRelativeMouseMode(SDL_TRUE);
	SDL_SetWindowGrab(g_Window, SDL_TRUE);
	SDL_ShowCursor(SDL_DISABLE);

    g_Renderer = CreateShaderAPI(); // Modular backend creation
    if (!g_Renderer || !g_Renderer->Init(g_Window, 1280, 720)) {
        std::cerr << "[Engine] ShaderAPI Init failed\n";
		Engine_Shutdown(); // make sure SDL is cleaned
        return;
    }

    g_Input.Init();  // Initialize our input system

    std::cout << "[Engine] SDL + ShaderAPI initialized\n";
}

//---FPS ------------------------------------------------------------------
// Uint64 now = SDL_GetPerformanceCounter(); // REMOVE IT 
// Uint64 last = now;
// double deltaTime = 0.0;

// FPS tracking
int frameCount = 0;
double fpsTimer = 0.0;
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Engine Frame: Main per-frame update and rendering
//-----------------------------------------------------------------------------
DLL_EXPORT bool STDCALL Engine_RunFrame(float deltaTime) {
    SDL_Event event;
    g_Input.Update(); // Reset and update input states, including mouse deltas
    
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            std::cout << "[Engine] SDL_QUIT event received\n";
            return false;
        }
    }

    // Input state and mouse movement
    const Uint8* keystate = g_Input.GetKeyState();
    int mouseDeltaX = g_Input.GetMouseDeltaX();
    int mouseDeltaY = g_Input.GetMouseDeltaY();

    // Update camera using input & deltaTime
    g_Camera.Update(deltaTime, keystate, mouseDeltaX, mouseDeltaY);

    int width, height;    // Get current window size
    SDL_GetWindowSize(g_Window, &width, &height);

    if (!g_Renderer) return false;

    g_Renderer->BeginFrame();
    g_Renderer->PrepareFrame(width, height);

    // --- STARFIELD START ---
    static float totalTime = 0.0f;
    totalTime += deltaTime;
    
    // Disable depth write and test just for starfield rendering
    g_Renderer->SetDepthMaskEnabled(false);
    g_Renderer->SetDepthTestEnabled(false);
    
    g_Renderer->RenderStarfield(totalTime);
    
    // Restore depth write and test for rest of the scene
    g_Renderer->SetDepthMaskEnabled(true);
    g_Renderer->SetDepthTestEnabled(true);

    // Update player logic with input
    g_Player.Update(deltaTime, g_Input);
    
    // Setup view matrix for rendering
    Matrix4x4_f view = g_Player.GetCamera().GetViewMatrix();
    g_Renderer->SetViewMatrix(view);
    
    // Draw static geometry loaded from map
    const auto& staticGeometry = GetStaticGeometry();
    for (const auto& instance : staticGeometry) {
        g_Renderer->DrawMesh(*instance.mesh, instance.transform);
    }

    g_Renderer->EndFrame();
    return true;
}

//-----------------------------------------------------------------------------
// Engine Shutdown: Clean up resources and shutdown SDL + Renderer + DLL
//-----------------------------------------------------------------------------
DLL_EXPORT void STDCALL Engine_Shutdown() {
    if (g_Renderer) {
        g_Renderer->Shutdown();
        DestroyShaderAPI();
        g_Renderer = nullptr;
    }

    if (g_Window) {
        SDL_DestroyWindow(g_Window);
        g_Window = nullptr;
    }

    g_Input.Shutdown();
    SDL_Quit();

    if (g_FileSystemDLL) {
        FreeLibrary(g_FileSystemDLL);
        g_FileSystemDLL = nullptr;
    }

    std::cout << "[Engine] Shutdown complete\n";
}

//-----------------------------------------------------------------------------
// Engine Main entrypoint: Init, load filesystem and map, run main loop
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

    Uint64 now = SDL_GetPerformanceCounter();
    Uint64 last = 0;
    double deltaTime = 0.0;

    while (true) {
        last = now;
        now = SDL_GetPerformanceCounter();
		
        // Convert performance counter difference to milliseconds
        deltaTime = (double)((now - last) * 1000 / (double)SDL_GetPerformanceFrequency());
		
		// Run one frame (deltaTime converted to seconds)
        bool keepRunning = Engine_RunFrame(static_cast<float>(deltaTime / 1000.0)); // seconds
		
        if (!keepRunning) {
            std::cout << "[Engine] Engine_RunFrame returned false, exiting loop\n";
            break;
        }
		
        // Yield CPU briefly
        SDL_Delay(1);
    }

	EngineLog("[Engine] Shutdown complete");
	EngineLog_Shutdown();
}

//-----------------------------------------------------------------------------
// Global camera accessor
//-----------------------------------------------------------------------------
Camera& GetMainCamera() {
    return g_Camera;
}