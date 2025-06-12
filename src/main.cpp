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
    // Step 1: Get full path to launcher EXE
    char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, MAX_PATH);
    std::string exeDir = std::filesystem::path(exePath).parent_path().string();
    std::string binDir = exeDir + "\\bin";

    // Step 2: Force DLL loading from local /bin only
    SetDllDirectoryA(binDir.c_str());

    // Step 3: Load engine.dll from absolute path inside /bin
    std::string engineDLLPath = binDir + "\\engine.dll";
    g_engineDLL = LoadLibraryExA(engineDLLPath.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
    if (!g_engineDLL) {
        std::cerr << "[Launcher] Failed to load engine.dll from: " << engineDLLPath << "\n";
        return false;
    }

    // Step 4: Bind engine functions
    Engine_Init      = (FnVoid)GetProcAddress(g_engineDLL, "Engine_Init");
    Engine_RunFrame  = (FnVoid)GetProcAddress(g_engineDLL, "Engine_RunFrame");
    Engine_Shutdown  = (FnVoid)GetProcAddress(g_engineDLL, "Engine_Shutdown");

    if (!Engine_Init || !Engine_RunFrame || !Engine_Shutdown) {
        std::cerr << "[Launcher] Failed to bind Engine entry points\n";
        return false;
    }

    std::cout << "[Launcher] engine.dll loaded and bound successfully\n";
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