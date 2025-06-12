
// Source Engine-style launcher
#include <Windows.h>
#include <SDL.h>
#include <glad/glad.h>
#include <iostream>
#include <filesystem>

#define SDL_MAIN_HANDLED

// DLL entry points
typedef void (__stdcall *FnVoid)();
static FnVoid Engine_Init = nullptr;
static FnVoid Engine_RunFrame = nullptr;
static FnVoid Engine_Shutdown = nullptr;

static HMODULE g_engineDLL = nullptr;

bool LoadEngineDLL() {
    SetDllDirectoryA("bin");

    g_engineDLL = LoadLibraryExA("engine.dll", NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
    if (!g_engineDLL) {
        DWORD err = GetLastError();
        std::cerr << "[Launcher] Failed to load engine.dll (err=" << err << ")\n";
        return false;
    }

    Engine_Init = (FnVoid)GetProcAddress(g_engineDLL, "Engine_Init");
    Engine_RunFrame = (FnVoid)GetProcAddress(g_engineDLL, "Engine_RunFrame");
    Engine_Shutdown = (FnVoid)GetProcAddress(g_engineDLL, "Engine_Shutdown");

    if (!Engine_Init || !Engine_RunFrame || !Engine_Shutdown) {
        std::cerr << "[Launcher] Failed to get Engine entry points\n";
        return false;
    }

    std::cout << "[Launcher] engine.dll loaded and bound successfully\n";
    return true;
}

extern "C" __declspec(dllexport) void Engine_Run() {
    // 🔍 VERSION + PATH DEBUG
    std::cout << "[VERSION] launcher build: v1.0.3\n";
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);
    std::cout << "[DEBUG] launcher.exe running from: " << path << "\n";

    SDL_SetMainReady();
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        std::cerr << "[Launcher] SDL_Init Error: " << SDL_GetError() << "\n";
        return;
    }

    if (!LoadEngineDLL()) return;

    std::cout << "[Launcher] Starting Engine_Init...\n";
    Engine_Init();

    std::cout << "[Launcher] Entering Engine_RunFrame loop...\n";
    while (true) {
        Engine_RunFrame();
        SDL_Delay(16); // approx 60 FPS
    }

    Engine_Shutdown();
    SDL_Quit();
}