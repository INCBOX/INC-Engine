// engine.cpp
// Core engine logic managing SDL window, OpenGL context, filesystem DLL loading,
// map loading, and main loop. Uses fully modular ShaderAPI abstraction for rendering.
// No direct OpenGL or Vulkan calls here — only through ShaderAPI interfaces.

// Includes Win32 API for dynamic DLL loading (LoadLibrary, GetProcAddress)
#include <Windows.h>

#define SDL_MAIN_HANDLED // BEFORE SDL
#include <SDL.h>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include "nlohmann/json.hpp"

#include "engine.h"

// Modular renderer interface
#include "renderer/renderer.h" // <--- CHANGED: Include the main Renderer interface

using json = nlohmann::json;

//-----------------------------------------------------------------------------
// FileSystem DLL dynamic loading handles and function pointers
// Allows modular filesystem implementation similar to Source Engine's filesystem_stdio.dll.
// We load this DLL at runtime and call its exported functions via pointers.
// This approach lets us swap or update the filesystem without recompiling engine.
//-----------------------------------------------------------------------------
static HMODULE g_FileSystemDLL = nullptr;

// Function pointer types for filesystem interface
typedef const std::string& (*FS_GetGameDirFn)();
typedef std::string (*FS_ResolvePathFn)(const std::string&);

// Function pointers to actual implementations loaded from DLL
static FS_GetGameDirFn FS_GetGameDir = nullptr;
static FS_ResolvePathFn FS_ResolvePath = nullptr;

//-----------------------------------------------------------------------------
// SDL Window and rendering handles
//-----------------------------------------------------------------------------
static SDL_Window* g_Window = nullptr;
static Renderer* g_Renderer = nullptr; // <--- CHANGED: Now points to the main Renderer class

//-----------------------------------------------------------------------------
// Platform-dependent export and calling conventions for DLL interface
// Ensures functions can be exported with correct symbol visibility and calling style.
//-----------------------------------------------------------------------------
#if defined(_WIN32)
#define DLL_EXPORT extern "C" __declspec(dllexport)
#define STDCALL __stdcall
#else
#define DLL_EXPORT extern "C" __attribute__((visibility("default")))
#define STDCALL
#endif

//-----------------------------------------------------------------------------
// Initialize engine log output file
// Redirects stdout and stderr to "logs/engine.log" for easier debugging.
// Also disables buffering to ensure immediate flush of logs to file/console.
//-----------------------------------------------------------------------------
void InitEngineLog() {
    std::filesystem::create_directories("logs");
    FILE* logFile = nullptr;
    freopen_s(&logFile, "logs/engine.log", "w", stdout); // Added missing semicolon
    if (logFile) {
        freopen_s(&logFile, "logs/engine.log", "a", stderr);
    } else {
        std::cerr << "[Engine] Failed to open stdout log file.\n";
    }

    setvbuf(stdout, nullptr, _IONBF, 0);
    setvbuf(stderr, nullptr, _IONBF, 0);
    std::cout << "[Engine] Log started\n";
}

//-----------------------------------------------------------------------------
// Dynamically loads the filesystem_stdio.dll which provides file path resolution
// and game directory functions.
// Returns true if DLL and functions loaded successfully.
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
        FreeLibrary(g_FileSystemDLL); // Clean up if exports fail
        g_FileSystemDLL = nullptr;
        return false;
    }

    std::cout << "[Engine] FileSystem loaded\n";
    return true;
}

//-----------------------------------------------------------------------------
// Loads a JSON map file from disk using the filesystem's resolved path.
// Parses entities and forwards the map data to the renderer module.
// Returns true on success, false on failure.
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

    // Forward map to renderer (future work)
    // g_Renderer->LoadMap(mapData); // Example of forwarding to the main renderer
    return true;
}

//-----------------------------------------------------------------------------
// Engine Initialization entry point
// Creates SDL window and OpenGL context,
// Initializes the ShaderAPI abstraction layer and renderer module.
// No direct graphics calls here, all done through interfaces.
//-----------------------------------------------------------------------------
DLL_EXPORT void STDCALL Engine_Init() {
    SDL_SetMainReady();

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        std::cerr << "[Engine] SDL_Init failed: " << SDL_GetError() << "\n";
        return;
    }

    g_Window = SDL_CreateWindow("INC Engine",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN); // Keep SDL_WINDOW_OPENGL for now as Renderer still relies on GL context
    if (!g_Window) {
        std::cerr << "[Engine] SDL_CreateWindow failed: " << SDL_GetError() << "\n";
        return;
    }

    g_Renderer = new Renderer(); // <--- CHANGED: Instantiate the main Renderer class
    if (!g_Renderer->Init(g_Window)) { // <--- CHANGED: Pass window handle
        std::cerr << "[Engine] Renderer init failed\n";
        return;
    }

    std::cout << "[Engine] SDL + Renderer initialized\n"; // <--- CHANGED: Reflects the new structure
}

//-----------------------------------------------------------------------------
// Engine frame update function
// Polls SDL events, handles quit requests,
// invokes renderer frame logic, and swaps buffers.
//-----------------------------------------------------------------------------
DLL_EXPORT bool STDCALL Engine_RunFrame() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            std::cout << "[Engine] Quit event received.\n";
            return false; // Signal main loop to exit
        }
        // Optionally add keyboard/mouse input here
    }

    int width, height;
    SDL_GetWindowSize(g_Window, &width, &height);

    if (!g_Renderer) {
        std::cerr << "[Engine] Renderer is null!\n";
        return false;
    }

    std::cout << "[Engine] Calling BeginFrame...\n";
    g_Renderer->BeginFrame();
    std::cout << "[Engine] BeginFrame complete. Calling RenderFrame...\n";
    g_Renderer->RenderFrame(width, height);
    std::cout << "[Engine] RenderFrame complete. Calling EndFrame...\n";
    g_Renderer->EndFrame(); // No window param here
    std::cout << "[Engine] EndFrame complete.\n";

    return true;
}

//-----------------------------------------------------------------------------
// Engine shutdown function
// Cleans up renderer, ShaderAPI, SDL context and window,
// unloads filesystem DLL.
//-----------------------------------------------------------------------------
DLL_EXPORT void STDCALL Engine_Shutdown() {
    if (g_Renderer) {
        g_Renderer->Shutdown(); // <--- CHANGED: Call Renderer's Shutdown
        delete g_Renderer;
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
// Main engine run loop entry point.
// Initializes logging, engine, filesystem, loads starting map,
// then enters continuous frame update loop with basic 60 FPS cap.
//-----------------------------------------------------------------------------
DLL_EXPORT void STDCALL Engine_Run() {
    InitEngineLog();
    std::cout << "[Engine] Starting Engine_Run\n";

    Engine_Init();

    if (!LoadFileSystem()) {
        std::cerr << "[Engine] Failed to load filesystem\n";
        Engine_Shutdown(); // Ensure proper shutdown if filesystem fails
        return;
    }

    // Example of using the loaded filesystem
    std::cout << "[Engine] Game directory: " << FS_GetGameDir() << "\n";


    if (!LoadMap("start")) {
        std::cerr << "[Engine] Failed to load start map\n";
        Engine_Shutdown(); // Ensure proper shutdown if map fails
        return;
    }

    std::cout << "[Engine] Entering main loop\n";

    while (true) {
        bool keepRunning = Engine_RunFrame();
        if (!keepRunning) {
            std::cout << "[Engine] Engine_RunFrame returned false, exiting loop\n";
            break;
        }
        SDL_Delay(16); // ~60 FPS cap
    }

    Engine_Shutdown();

    std::cout << "[Engine] Engine_Run exiting\n";
}