#pragma once

#include <string>
#include <optional>
#include <filesystem>

struct FsLaunchInfo {
    std::filesystem::path exeDir;
    std::filesystem::path gameDir;
    std::filesystem::path gameinfoPath;
    std::string fsDllPath;
    std::string engineDllPath;
	std::string gameTitle;
};

// Resolves executable dir, game dir, gameinfo.txt path, and DLL paths.
// Returns std::nullopt on failure, printing error to stderr.
std::optional<FsLaunchInfo> ResolveFilesystemLaunchInfo(int argc, char* argv[]);