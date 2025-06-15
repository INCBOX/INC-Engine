// main.cpp — INC Launcher (Source-style, cross-platform)
// Dynamically loads the engine library at runtime, depending on OS:
// Windows:  engine.dll
// Linux:    libengine.so
// macOS:    libengine.dylib
//
// Responsible for loading the engine, binding its core entry points, and running the main loop.
//
// This launcher expects the engine binaries to be located in a 'bin' subfolder relative to the executable.
//

#include <iostream>
#include <string>
#include <filesystem>
#include <thread>  // For std::this_thread::sleep_for
#include <chrono>  // For std::chrono::milliseconds

#if defined(_WIN32)
    #include <Windows.h>
    typedef void (__stdcall *FnVoid)(); // __stdcall calling convention for Windows DLL exports
#else
    #include <dlfcn.h>
    typedef void (*FnVoid)();           // Default calling convention for Linux/macOS shared libs
#endif

// Function pointers to engine entry points
static FnVoid Engine_Init = nullptr;
static FnVoid Engine_RunFrame = nullptr;
static FnVoid Engine_Shutdown = nullptr;

// Handle to the loaded engine library
#if defined(_WIN32)
    static HMODULE g_engineLib = nullptr;
#else
    static void* g_engineLib = nullptr;
#endif

// Loads the engine dynamic library from disk and binds core function pointers.
// Returns true if all succeed, false on failure.
bool LoadEngineLibrary() {
    std::string enginePath;

#if defined(_WIN32)
    // --- Windows specific library loading ---
    
    // Get the directory of the current executable
    char exePath[MAX_PATH];
    if (!GetModuleFileNameA(NULL, exePath, MAX_PATH)) {
        std::cerr << "[Launcher] GetModuleFileNameA failed\n";
        return false;
    }

    std::string exeDir = std::filesystem::path(exePath).parent_path().string();
    std::string binDir = exeDir + "\\bin";

    // Add 'bin' directory to DLL search path to ensure dependencies are found
    if (!SetDllDirectoryA(binDir.c_str())) {
        std::cerr << "[Launcher] Failed to set DLL directory to " << binDir << "\n";
        return false;
    }

    // Compose full path to engine DLL
    enginePath = binDir + "\\engine.dll";

    // Load the DLL with altered search path flag for proper dependency loading
    g_engineLib = LoadLibraryExA(enginePath.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
    if (!g_engineLib) {
        std::cerr << "[Launcher] Failed to load engine.dll from " << enginePath << "\n";
        return false;
    }

    // Get the engine core entry points by name
    Engine_Init      = (FnVoid)GetProcAddress(g_engineLib, "Engine_Init");
    Engine_RunFrame  = (FnVoid)GetProcAddress(g_engineLib, "Engine_RunFrame");
    Engine_Shutdown  = (FnVoid)GetProcAddress(g_engineLib, "Engine_Shutdown");

#else
    // --- Linux and macOS dynamic library loading ---
    
    // Determine shared library extension and path depending on platform
    #if defined(__APPLE__)
        enginePath = "./bin/libengine.dylib";
    #else
        enginePath = "./bin/libengine.so";
    #endif

    // Load the shared library with RTLD_NOW to resolve symbols immediately
    g_engineLib = dlopen(enginePath.c_str(), RTLD_NOW);
    if (!g_engineLib) {
        std::cerr << "[Launcher] Failed to load engine shared library: " << dlerror() << "\n";
        return false;
    }

    // Bind the engine entry points using dlsym
    Engine_Init      = (FnVoid)dlsym(g_engineLib, "Engine_Init");
    Engine_RunFrame  = (FnVoid)dlsym(g_engineLib, "Engine_RunFrame");
    Engine_Shutdown  = (FnVoid)dlsym(g_engineLib, "Engine_Shutdown");

#endif

    // Verify all required entry points were found
    if (!Engine_Init || !Engine_RunFrame || !Engine_Shutdown) {
        std::cerr << "[Launcher] Failed to bind engine entry points\n";
        return false;
    }

    std::cout << "[Launcher] Engine library loaded and entry points bound successfully\n";
    return true;
}

// Unloads the engine library and resets all pointers to null
void UnloadEngineLibrary() {
#if defined(_WIN32)
    if (g_engineLib) {
        FreeLibrary(g_engineLib);
        g_engineLib = nullptr;
    }
#else
    if (g_engineLib) {
        dlclose(g_engineLib);
        g_engineLib = nullptr;
    }
#endif

    Engine_Init = nullptr;
    Engine_RunFrame = nullptr;
    Engine_Shutdown = nullptr;
}

int main() {
    std::cout << "[INC Launcher] Cross-platform launcher started\n";

    // Load the engine DLL / shared library and bind functions
    if (!LoadEngineLibrary()) {
        std::cerr << "[Launcher] Failed to load engine library\n";
        return -1;
    }

    // Initialize the engine
    Engine_Init();

    // Main loop - runs until externally terminated (e.g., user closes window or engine signals exit)
	while (true) {
		Engine_RunFrame();

		// Sleep roughly 16ms per frame to cap at ~60 FPS and prevent 100% CPU usage.
		// This is a simple frame pacing method; can be refined with precise timing if needed.
		std::this_thread::sleep_for(std::chrono::milliseconds(16));
	}

    // This point is never reached in current design
    Engine_Shutdown();

    UnloadEngineLibrary();

    return 0;
}