//-----------------------------------------------------------------------------
// ENGINE.CPP - INC. INC© INCBOX 2007 ALL RIGHTS RESERVED.
// Core engine logic: SDL window, ShaderAPI abstraction, map loading (JSON),
// input, player, and main loop.
//-----------------------------------------------------------------------------

#include <Windows.h>


#include <SDL2/SDL.h>

#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include "nlohmann/json.hpp"

#include "engine_dll_api.h"				// Used by launcher/editor to call engine DLL
#include "engine_dll_internal.h"		// Declares internal-only functions

#include "engine_filesystem.h"          // now just the interface
#include "engine_globals.h"             // access the main camera from anywhere in engine
#include "engine_log.h"

#include "engine_renderer.h"
#include "engine_renderer_dll.h"
#include "fps_counter.h"

#include "player.h"
#include "input.h"
#include "camera_manager.h"
#include "mathlib/matrix4x4_f.h"

#include "world/static_mesh_loader.h"   // Static geometry loader (JSON)

using json = nlohmann::json;

//-----------------------------------------------------------------------------
// Globals
//-----------------------------------------------------------------------------
Input g_Input;
static Player g_Player;
static SDL_Window* g_Window = nullptr;
static FPSCounter g_FPSCounter;

CameraManager g_CameraManager;

//-----------------------------------------------------------------------------
// Logging macros and setup
//-----------------------------------------------------------------------------
#ifdef DEBUG
#define LOG(x) std::cout << x << std::endl;
#else
#define LOG(x)
#endif

void InitEngineLog() {
    EngineLog_Init();
    EngineLog("[Engine] Log started");
}

//-----------------------------------------------------------------------------
// Accessors
//-----------------------------------------------------------------------------
CameraManager& GetCameraManager() {
    return g_CameraManager;
}

Camera_f& GetMainCamera_f() {
    return g_CameraManager.GetCamera_f();
}

//-----------------------------------------------------------------------------
// Map loading and parsing (JSON)
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
// Initialization
//-----------------------------------------------------------------------------
bool Engine_Internal_Init(void* nativeWindowHandle) {
    SDL_SetMainReady();

    g_Window = static_cast<SDL_Window*>(nativeWindowHandle);
    if (!g_Window) {
        std::cerr << "[Engine] Invalid window handle passed to Engine_Init\n";
        return false;
    }

    // Since window is created externally, do NOT create window here
    // SDL_Init still needed for subsystems

	// SDL_Init already called by launcher; skip here

    SDL_SetRelativeMouseMode(SDL_TRUE);
    SDL_SetWindowGrab(g_Window, SDL_TRUE);
    SDL_ShowCursor(SDL_DISABLE);

    if (!RendererDLL_LoadAndInit(g_Window)) {
        std::cerr << "[Engine] Failed to initialize Renderer\n";
        return false;
    }

    // Now loading map will resolve paths correctly
    if (!LoadMap("start")) {
        std::cerr << "[Engine] Failed to load map: start\n";
        // optionally continue or fail here
    }

    g_Input.Init();

    std::cout << "[Engine] SDL + ShaderAPI initialized with external window\n";

    return true;
}

//-----------------------------------------------------------------------------
// Per-frame update
//-----------------------------------------------------------------------------
void UpdateInputAndCamera(float deltaTime) {
    g_CameraManager.SetPrecision(CameraPrecision::Double);

    int mouseDeltaX = g_Input.GetMouseDeltaX();
    int mouseDeltaY = g_Input.GetMouseDeltaY();

    g_CameraManager.UpdateRotationOnly(deltaTime, mouseDeltaX, mouseDeltaY);

    auto& cam_d = g_CameraManager.GetCamera_d();
    auto& cam_f = g_CameraManager.GetCamera_f();

    cam_f.SetYaw(static_cast<float>(cam_d.GetYaw()));
    cam_f.SetPitch(static_cast<float>(cam_d.GetPitch()));
    cam_f.ClampPitch(-89.9f, 89.9f);
    cam_f.UpdateOrientation();
}

//-----------------------------------------------------------------------------
// Rendering
//-----------------------------------------------------------------------------
void RenderFrame(float deltaTime) {
    int width, height;
    SDL_GetWindowSize(g_Window, &width, &height);

    static float totalTime = 0.0f;
    totalTime += deltaTime;

    Matrix4x4_f viewMatrix = g_CameraManager.GetLocalViewMatrix();
    Matrix4x4_f projMatrix = Matrix4x4_f::Perspective(70.0f, (float)width / height, 0.01f, 1000.0f);

    Renderer_RenderFrame(viewMatrix, projMatrix, totalTime);
}

//-----------------------------------------------------------------------------
// Engine per-frame run
//-----------------------------------------------------------------------------
bool Engine_Internal_RunFrame(float deltaTime) {
    // Important: call g_Input.Update() here inside engine_dll.cpp or engine.cpp
    g_Input.Update();

    // existing update calls
    UpdateInputAndCamera(deltaTime);
    g_Player.Update(deltaTime, g_Input);
    RenderFrame(deltaTime);

    return true;
}

//-----------------------------------------------------------------------------
// Shutdown and cleanup
//-----------------------------------------------------------------------------
void Engine_Internal_Shutdown() {
    RendererDLL_Unload();


    g_Input.Shutdown();
    // Don't destroy window or quit SDL, launcher handles it.

    // Unload filesystem handled inside engine_filesystem.cpp
    UnloadFileSystem();

    std::cout << "[Engine] Shutdown complete\n";
}