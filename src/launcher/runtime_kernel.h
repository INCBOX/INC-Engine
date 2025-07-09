#pragma once

#include "dll_platform.h"

#include <string>
#include <unordered_map>

using FSInitFn = bool(*)(const std::string&);
using EngineInitWithFSFn = bool(*)(void*);
using EngineInitFnNew = bool(*)(void*);
using EngineRunFrameFn = bool(*)(float);
using EngineShutdownFn = void(*)();
using EngineProcessSDLEventFn = void(*)(void* /*SDL_Event*/);

class RuntimeKernel {
public:
    RuntimeKernel();
    ~RuntimeKernel();

    // Initialize kernel with paths, load filesystem and engine modules
    bool Initialize(const std::string& fsDllPath, const std::string& engineDllPath, const std::string& gameinfoPath);

    // Run the main loop (poll events, run frame, handle shutdown)
    int RunMainLoop(const std::string& windowTitle);

    // Shutdown and unload modules
    void Shutdown();

private:
    bool LoadFilesystem(const std::string& path, const std::string& gameinfoPath);
    bool LoadEngine(const std::string& path);

    void UnloadModules();

    LibHandle fsLib = nullptr;
    LibHandle engineLib = nullptr;

    FSInitFn FS_Init = nullptr;

    EngineInitWithFSFn Engine_InitWithFS_ = nullptr;
    EngineInitFnNew Engine_Init = nullptr;
    EngineRunFrameFn Engine_RunFrame = nullptr;
    EngineShutdownFn Engine_Shutdown = nullptr;
    EngineProcessSDLEventFn Engine_ProcessSDLEvent = nullptr;
};