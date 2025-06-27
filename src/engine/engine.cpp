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
#include "engine_globals.h"              // access the main camera from anywhere in engine
#include "engine_log.h"

#include "engine_renderer.h"

#include "world/static_mesh_loader.h"    // Static geometry loader (JSON)

#include "input.h"
#include "camera_manager.h"
#include "mathlib/matrix4x4_f.h"

#include "player.h"

using json = nlohmann::json;

CameraManager g_CameraManager;

CameraManager& GetCameraManager() {
    return g_CameraManager;
}

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
static Input g_Input;
static Player g_Player;
static SDL_Window* g_Window = nullptr;

//-----------------------------------------------------------------------------
// FileSystem DLL dynamic loading
//-----------------------------------------------------------------------------
static HMODULE g_FileSystemDLL = nullptr;

typedef const std::string& (*FS_GetGameDirFn)();
typedef std::string (*FS_ResolvePathFn)(const std::string&);
static FS_GetGameDirFn FS_GetGameDir = nullptr;
static FS_ResolvePathFn FS_ResolvePath = nullptr;

//-----------------------------------------------------------------------------
// ShaderAPI DLL dynamic loading
//-----------------------------------------------------------------------------
static HMODULE g_ShaderAPIDLL = nullptr;

typedef IGPURenderInterface* (*CreateGPUAPI_t)();
typedef void (*DestroyGPUAPI_t)();

static CreateGPUAPI_t pCreateGPUAPI = nullptr;
static DestroyGPUAPI_t pDestroyGPUAPI = nullptr;
// Don't extern from the DLL — define your own pointer here:
static IGPURenderInterface* s_pGPURender = nullptr;


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

    FS_GetGameDir = reinterpret_cast<FS_GetGameDirFn>(GetProcAddress(g_FileSystemDLL, "FS_GetGameDir"));
    FS_ResolvePath = reinterpret_cast<FS_ResolvePathFn>(GetProcAddress(g_FileSystemDLL, "FS_ResolvePath"));

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
// Load shaderapi.dll and get function pointers + global pointer
//-----------------------------------------------------------------------------
bool LoadShaderAPIDLL() {
    g_ShaderAPIDLL = LoadLibraryA("bin/shaderapi.dll");
    if (!g_ShaderAPIDLL) {
        std::cerr << "[Engine] Failed to load shaderapi.dll\n";
        return false;
    }

    pCreateGPUAPI = reinterpret_cast<CreateGPUAPI_t>(GetProcAddress(g_ShaderAPIDLL, "CreateGPUAPI"));
    pDestroyGPUAPI = reinterpret_cast<DestroyGPUAPI_t>(GetProcAddress(g_ShaderAPIDLL, "DestroyGPUAPI"));

    if (!pCreateGPUAPI || !pDestroyGPUAPI) {
        std::cerr << "[Engine] Failed to get shaderapi exports\n";
        FreeLibrary(g_ShaderAPIDLL);
        g_ShaderAPIDLL = nullptr;
        return false;
    }

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

    // Enable relative mouse mode: grab and hide cursor for FPS controls
    SDL_SetRelativeMouseMode(SDL_TRUE);
    SDL_SetWindowGrab(g_Window, SDL_TRUE);
    SDL_ShowCursor(SDL_DISABLE);

    if (!LoadShaderAPIDLL()) {
        std::cerr << "[Engine] Failed to load ShaderAPI DLL\n";
        Engine_Shutdown();
        return;
    }
	
    // **THIS IS THE MEAL TIME: Create GPU render interface**
    s_pGPURender = pCreateGPUAPI(); // Set it after loading the DLL
	
	if (!s_pGPURender || !s_pGPURender->Init(g_Window, 1280, 720)) {
		printf("Failed to initialize GPU backend!\n");
		return;
	}

	Renderer_Init(s_pGPURender, g_Window);

    g_Input.Init();  // Initialize input system

    std::cout << "[Engine] SDL + ShaderAPI initialized\n";
}

//-----------------------------------------------------------------------------
// Engine Frame: Main per-frame update and rendering
//-----------------------------------------------------------------------------
DLL_EXPORT bool STDCALL Engine_RunFrame(float deltaTime) {
    SDL_Event event;
    g_Input.Update();

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            std::cout << "[Engine] SDL_QUIT event received\n";
            return false;
        }
    }

    // Force double precision camera
    g_CameraManager.SetPrecision(CameraPrecision::Double);

    // Get mouse deltas
    const Uint8* keystate = g_Input.GetKeyState();
    (void)keystate; // unused to silence warning
    int mouseDeltaX = g_Input.GetMouseDeltaX();
    int mouseDeltaY = g_Input.GetMouseDeltaY();

    // Update camera rotation (double precision)
    g_CameraManager.UpdateRotationOnly(deltaTime, mouseDeltaX, mouseDeltaY);

    // Sync float camera rotation from double camera
    {
        auto& cam_d = g_CameraManager.GetCamera_d();
        auto& cam_f = g_CameraManager.GetCamera_f();

        cam_f.SetYaw(static_cast<float>(cam_d.GetYaw()));
        cam_f.SetPitch(static_cast<float>(cam_d.GetPitch()));
        cam_f.ClampPitch(-89.9f, 89.9f);
        cam_f.UpdateOrientation();
    }

    g_Player.Update(deltaTime, g_Input);
	
	
	int width, height;
	SDL_GetWindowSize(g_Window, &width, &height);
	
	static float totalTime = 0.0f;
	totalTime += deltaTime;
	
	Matrix4x4_f viewMatrix = g_CameraManager.GetLocalViewMatrix();
	Matrix4x4_f projMatrix = Matrix4x4_f::Perspective(70.0f, (float)width / height, 0.01f, 1000.0f);
	
	Renderer_RenderFrame(viewMatrix, projMatrix, totalTime);


    return true;
}

//-----------------------------------------------------------------------------
// Engine Shutdown: Cleanup SDL + Renderer + DLL
//-----------------------------------------------------------------------------
DLL_EXPORT void STDCALL Engine_Shutdown() {
	Renderer_Shutdown();

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

    if (g_ShaderAPIDLL) {
        FreeLibrary(g_ShaderAPIDLL);
        g_ShaderAPIDLL = nullptr;
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
        bool keepRunning = Engine_RunFrame(static_cast<float>(deltaTime / 1000.0f));

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
Camera_f& GetMainCamera_f() {
    return g_CameraManager.GetCamera_f();
}

IGPURenderInterface* GetRenderInterface() {
    return s_pGPURender;
}