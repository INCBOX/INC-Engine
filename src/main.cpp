// main.cpp — INC Launcher (Source-style)
// ONLY loads engine.dll

#include <Windows.h>
#include <iostream>

typedef void (__stdcall *FnVoid)();

static FnVoid Engine_Init = nullptr;
static FnVoid Engine_RunFrame = nullptr;
static FnVoid Engine_Shutdown = nullptr;

static HMODULE g_engineDLL = nullptr;

bool LoadEngineDLL() {
    SetDllDirectoryA("bin");

    g_engineDLL = LoadLibraryExA("engine.dll", NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
    if (!g_engineDLL) {
        std::cerr << "[Launcher] Failed to load engine.dll\n";
        return false;
    }

    Engine_Init      = (FnVoid)GetProcAddress(g_engineDLL, "Engine_Init");
    Engine_RunFrame  = (FnVoid)GetProcAddress(g_engineDLL, "Engine_RunFrame");
    Engine_Shutdown  = (FnVoid)GetProcAddress(g_engineDLL, "Engine_Shutdown");

    if (!Engine_Init || !Engine_RunFrame || !Engine_Shutdown) {
        std::cerr << "[Launcher] Failed to bind Engine entry points\n";
        return false;
    }

    return true;
}

int main() {
    std::cout << "[INC Launcher] Source-style start\n";

    if (!LoadEngineDLL()) return -1;

    Engine_Init();

    while (true)
        Engine_RunFrame();

    Engine_Shutdown();
    return 0;
}