// engine.cpp - updated for Source-style folder layout
#include <Windows.h>
#include <SDL.h>
#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include "nlohmann/json.hpp"  // JSON parser
using json = nlohmann::json;

static HMODULE g_FileSystemDLL = nullptr;
typedef const std::string& (*FS_GetGameDirFn)();
typedef std::string (*FS_ResolvePathFn)(const std::string&);
static FS_GetGameDirFn FS_GetGameDir = nullptr;
static FS_ResolvePathFn FS_ResolvePath = nullptr;

static SDL_Window* g_Window = nullptr;
static SDL_GLContext g_GLContext = nullptr;

#if defined(_WIN32)
#define DLL_EXPORT extern "C" __declspec(dllexport)
#define STDCALL __stdcall
#else
#define DLL_EXPORT extern "C" __attribute__((visibility("default")))
#define STDCALL
#endif

void InitEngineLog() {
    CreateDirectoryA("logs", NULL);
    FILE* logFile;
    freopen_s(&logFile, "logs/engine.log", "w", stdout);
    freopen_s(&logFile, "logs/engine.log", "w", stderr);
    setvbuf(stdout, nullptr, _IONBF, 0);
    setvbuf(stderr, nullptr, _IONBF, 0);
    std::cout << "[Engine] Log started\n";
}

bool LoadFileSystem() {
    g_FileSystemDLL = LoadLibraryExA("bin/filesystem_stdio.dll", NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
    if (!g_FileSystemDLL) {
        std::cerr << "[Engine] Failed to load filesystem_stdio.dll\n";
        return false;
    }

    FS_GetGameDir = (FS_GetGameDirFn)GetProcAddress(g_FileSystemDLL, "FS_GetGameDir");
    FS_ResolvePath = (FS_ResolvePathFn)GetProcAddress(g_FileSystemDLL, "FS_ResolvePath");

    std::cout << "[Engine] FS_GetGameDir ptr: " << (void*)FS_GetGameDir << "\n";
    std::cout << "[Engine] FS_ResolvePath ptr: " << (void*)FS_ResolvePath << "\n";

    if (!FS_GetGameDir || !FS_ResolvePath) {
        std::cerr << "[Engine] Failed to resolve FileSystem exports:\n";
        if (!FS_GetGameDir) std::cerr << " - FS_GetGameDir is null\n";
        if (!FS_ResolvePath) std::cerr << " - FS_ResolvePath is null\n";
        return false;
    }

    std::cout << "[Engine] FileSystem loaded and linked\n";
    return true;
}

DLL_EXPORT void STDCALL Engine_Init() {
    std::cout << "[Engine] Engine_Init called\n";
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
    if (!g_GLContext || !gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        std::cerr << "[Engine] Failed to initialize GLAD or OpenGL context\n";
        return;
    }

    std::cout << "[Engine] Initialized SDL + OpenGL successfully\n";
}

bool LoadMap(const std::string& mapName) {
    std::string relative = "maps/" + mapName + ".json";
    std::cout << "[Engine] Attempting to load map: " << relative << "\n";

    std::string resolved = FS_ResolvePath(relative);
    std::cout << "[Engine] FS_ResolvePath returned: " << resolved << "\n";

    if (resolved.empty()) {
        std::cerr << "[Engine] ERROR: Map not found: " << relative << "\n";
        return false;
    }

    std::ifstream mapFile(resolved);
    if (!mapFile.is_open()) {
        std::cerr << "[Engine] ERROR: Failed to open map file: " << resolved << "\n";
        return false;
    }

    json mapData;
    try {
        mapFile >> mapData;
    } catch (const std::exception& e) {
        std::cerr << "[Engine] ERROR: JSON parsing failed: " << e.what() << "\n";
        return false;
    }

    std::cout << "[Engine] Successfully loaded map file: " << resolved << std::endl;

    if (mapData.contains("entities") && mapData["entities"].is_array()) {
        for (const auto& entity : mapData["entities"]) {
            std::string classname = entity.value("classname", "unknown");
            auto origin = entity.value("origin", std::vector<float>{0, 0, 0});
            std::cout << "[Entity] " << classname << " at ("
                      << origin[0] << ", " << origin[1] << ", " << origin[2] << ")\n";
        }
    }

    return true;
}

DLL_EXPORT void STDCALL Engine_RunFrame() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            std::cout << "[Engine] SDL_QUIT received, exiting\n";
            exit(0);
        }
    }

    glClearColor(0.1f, 0.1f, 0.25f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    SDL_GL_SwapWindow(g_Window);
}

DLL_EXPORT void STDCALL Engine_Shutdown() {
    std::cout << "[Engine] Engine_Shutdown called\n";
    if (g_GLContext) SDL_GL_DeleteContext(g_GLContext);
    if (g_Window) SDL_DestroyWindow(g_Window);
    SDL_Quit();
    std::cout << "[Engine] Shutdown complete\n";
}

DLL_EXPORT void STDCALL Engine_Run() {
    InitEngineLog();
    std::cout << "[Engine] Engine_Run started\n";

    Engine_Init();

    if (!LoadFileSystem()) {
        std::cerr << "[Engine] Could not initialize filesystem\n";
        return;
    }

    if (!LoadMap("start")) {
        std::cerr << "[Engine] LoadMap failed. Exiting.\n";
        return;
    }

    while (true) {
        Engine_RunFrame();
        SDL_Delay(16);
    }

    Engine_Shutdown();
}