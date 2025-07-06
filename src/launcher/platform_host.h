#pragma once

#include <string>

// Runs the platform host (window, SDL, engine DLL loading, main loop)
// - engineDllPath: path to the engine DLL/shared lib
// - fsDllPath: path to the filesystem DLL/shared lib
// - gameInfoPath: path to the gameinfo.txt file (needed for FS_Init)
// - windowTitle: window title string (e.g. "INC Engine")
// Returns 0 on success, negative error code on failure.
int PlatformHost_Run(
    const std::string& engineDllPath,
    const std::string& fsDllPath,
    const std::string& gameInfoPath,
    const std::string& windowTitle);
	
	