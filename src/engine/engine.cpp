// engine.cpp
// Core engine logic managing SDL window, OpenGL context, filesystem DLL loading,
// map loading, and main loop. Uses fully modular ShaderAPI abstraction for rendering.
// No direct OpenGL or Vulkan calls here — only through ShaderAPI interfaces.

// Includes Win32 API for dynamic DLL loading (LoadLibrary, GetProcAddress)
#include <Windows.h>

#define SDL_MAIN_HANDLED
#include <SDL.h>

#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include "nlohmann/json.hpp"

#include "engine/render/render_sys.h"	 // Abstract renderer interface (platform-agnostic)
#include "shaderapi.h"                   // ShaderAPI core interface (OpenGL/Vulkan/DirectX abstraction)

using json = nlohmann::json;

//-----------------------------------------------------------------------------
// FileSystem DLL dynamic loading handles and function pointers
// Allows modular filesystem implementation similar to Source Engine's filesystem_stdio.dll.
// We load this DLL at runtime and call its exported functions via pointers.
//-----------------------------------------------------------------------------
static HMODULE g_FileSystemDLL = nullptr;

// Function pointer types for filesystem interface
typedef const std::string& (*FS_GetGameDirFn)();
typedef std::string (*FS_ResolvePathFn)(const std::string&);

// Function pointers to actual implementations loaded from DLL
static FS_GetGameDirFn FS_GetGameDir = nullptr;
static FS_ResolvePathFn FS_ResolvePath = nullptr;

//-----------------------------------------------------------------------------
// SDL Window and OpenGL context handles
//-----------------------------------------------------------------------------
static SDL_Window* g_Window = nullptr;
static SDL_GLContext g_GLContext = nullptr;

//-----------------------------------------------------------------------------
// Platform-dependent export and calling conventions for DLL interface
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
// Also disables buffering to ensure immediate flush of logs.
//-----------------------------------------------------------------------------
void InitEngineLog() {
    std::filesystem::create_directories("logs");
    FILE* logFile;
    // freopen_s is safer than freopen (Windows-specific)
    freopen_s(&logFile, "logs/engine.log", "w", stdout);
    freopen_s(&logFile, "logs/engine.log", "w", stderr);
	
    // Disable buffering on stdout/stderr to get real-time log output
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
	// Load the DLL from bin folder relative to executable
    g_FileSystemDLL = LoadLibraryExA("bin/filesystem_stdio.dll", NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
    if (!g_FileSystemDLL) {
        std::cerr << "[Engine] Failed to load filesystem_stdio.dll\n";
        return false;
    }

    // Retrieve function pointers to exported functions
    FS_GetGameDir = (FS_GetGameDirFn)GetProcAddress(g_FileSystemDLL, "FS_GetGameDir");
    FS_ResolvePath = (FS_ResolvePathFn)GetProcAddress(g_FileSystemDLL, "FS_ResolvePath");

    if (!FS_GetGameDir || !FS_ResolvePath) {
        std::cerr << "[Engine] Failed to resolve FileSystem exports\n";
        return false;
    }

    std::cout << "[Engine] FileSystem loaded, game dir: " << FS_GetGameDir() << "\n";
    return true;
}

//-----------------------------------------------------------------------------
// Loads a JSON map file from disk using the filesystem's resolved path.
// Parses entities and forwards the map data to the renderer module.
// Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool LoadMap(const std::string& mapName) {
    // Build relative path to map file
    std::string relative = "maps/" + mapName + ".json";
	
    // Resolve the full path using filesystem DLL
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

    // Debug: Print entities from the map to verify loading
    if (mapData.contains("entities") && mapData["entities"].is_array()) {
        for (const auto& entity : mapData["entities"]) {
            std::string classname = entity.value("classname", "unknown");
            auto origin = entity.value("origin", std::vector<float>{0, 0, 0});
            std::cout << "[Entity] " << classname << " at ("
                      << origin[0] << ", " << origin[1] << ", " << origin[2] << ")\n";
        }
    }

    // Pass the loaded map JSON to the renderer/game logic layer for further processing
    Render_LoadMap(mapData);

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

    g_Window = SDL_CreateWindow("SourceClone Engine",
                                SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (!g_Window) {
        std::cerr << "[Engine] SDL_CreateWindow failed: " << SDL_GetError() << "\n";
        return;
    }

    g_GLContext = SDL_GL_CreateContext(g_Window);
    if (!g_GLContext) {
        std::cerr << "[Engine] SDL_GL_CreateContext failed: " << SDL_GetError() << "\n";
        return;
    }

    std::cout << "[Engine] SDL + OpenGL context created\n";

    // Initialize the shader API abstraction layer (OpenGL backend for now)
    INC::InitShaderAPI();

    // Initialize renderer which drives rendering each frame
    Render_Init();
}

//-----------------------------------------------------------------------------
// Engine frame update function
// Polls SDL events, handles quit requests,
// invokes renderer frame logic, and swaps buffers.
//-----------------------------------------------------------------------------
DLL_EXPORT void STDCALL Engine_RunFrame() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            std::cout << "[Engine] SDL_QUIT received\n";
            exit(0);
        }
    }

    int width, height;
    SDL_GetWindowSize(g_Window, &width, &height);

    // Render current frame using abstracted renderer
    RenderFrame(width, height);

    // Present the rendered frame
    SDL_GL_SwapWindow(g_Window);
}

//-----------------------------------------------------------------------------
// Engine shutdown function
// Cleans up renderer, ShaderAPI, SDL context and window,
// unloads filesystem DLL.
//-----------------------------------------------------------------------------
DLL_EXPORT void STDCALL Engine_Shutdown() {
    std::cout << "[Engine] Engine_Shutdown called\n";

    Render_Shutdown();
    INC::ShutdownShaderAPI();

    if (g_GLContext) {
        SDL_GL_DeleteContext(g_GLContext);
        g_GLContext = nullptr;
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
}

//-----------------------------------------------------------------------------
// Main engine run loop entry point.
// Initializes logging, engine, filesystem, loads starting map,
// then enters continuous frame update loop with basic 60 FPS cap.
//-----------------------------------------------------------------------------
DLL_EXPORT void STDCALL Engine_Run() {
    InitEngineLog();

    Engine_Init();

    if (!LoadFileSystem()) {
        std::cerr << "[Engine] Failed to load filesystem\n";
        return;
    }
    if (!LoadMap("start")) {
        std::cerr << "[Engine] Failed to load start map\n";
        return;
    }

    while (true) {
        Engine_RunFrame();
        SDL_Delay(16); // ~60 FPS cap, consider better timers or vsync in future
    }

    Engine_Shutdown();
}