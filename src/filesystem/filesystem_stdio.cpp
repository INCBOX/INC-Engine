#include "inc_dll_utils.h"
#include "filesystem.h"
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;

// Internal state
static std::string g_gameDir;
static std::vector<std::string> g_searchPaths;

static std::string Trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    size_t last = str.find_last_not_of(" \t\n\r");
    return (first == std::string::npos) ? "" : str.substr(first, last - first + 1);
}

// ----------------------------
// C++ INTERFACE IMPLEMENTATION
// ----------------------------

class CFileSystem_Stdio : public IFileSystem {
public:
    std::string ResolvePath(const std::string& relativePath) override {
        std::cout << "[FS] Resolving (IFileSystem): " << relativePath << "\n";
        for (const auto& base : g_searchPaths) {
            fs::path full = fs::path(g_gameDir) / base / relativePath;
            std::cout << "[FS] Checking: " << full << "\n";
            if (fs::exists(full)) {
                std::cout << "[FS] Found: " << full << "\n";
                return full.string();
            }
        }
        std::cout << "[FS] Failed to resolve: " << relativePath << "\n";
        return "";
    }
};

// -----------------------------
// C++ INTERFACE EXPORT FUNCTION
// -----------------------------

extern "C" DLL_EXPORT IFileSystem* GetFileSystemInterface() {
    static CFileSystem_Stdio s_interface;
    return &s_interface;
}

// ---------------------
// C API IMPLEMENTATION
// ---------------------

extern "C" __declspec(dllexport) bool FS_Init(const std::string& gameinfo_path) {
    // Reset internal state if needed
    g_gameDir.clear();
    g_searchPaths.clear();

    fs::path gameinfo(gameinfo_path);
    if (!fs::exists(gameinfo)) {
        std::cerr << "[FS] FS_Init: gameinfo.txt not found: " << gameinfo_path << std::endl;
        return false;
    }

    // Use the existing LoadGameInfo logic
    CFileSystem_Stdio tempFS;  // Temporary instance to call LoadGameInfo (but LoadGameInfo is private)

    // Since LoadGameInfo is private, move the loading logic here or
    // make LoadGameInfo static or public, or move it outside class.

    // For now, copy-paste your LoadGameInfo logic here:

    g_gameDir = fs::absolute(gameinfo).parent_path().string();

    std::ifstream file(gameinfo_path);
    if (!file.is_open()) {
        std::cerr << "[FS] FS_Init failed to open gameinfo.txt: " << gameinfo_path << std::endl;
        return false;
    }

    std::string line;
    bool inFileSystem = false;
    bool inSearchPaths = false;

    while (std::getline(file, line)) {
        line = Trim(line);
        if (line.empty() || line[0] == '/' || line[0] == '\\') continue;

        if (line.find("FileSystem") != std::string::npos) inFileSystem = true;
        if (inFileSystem && line.find("SearchPaths") != std::string::npos) inSearchPaths = true;

        if (inSearchPaths && line.find("}") != std::string::npos) break;

        if (inSearchPaths && line.find("game") != std::string::npos) {
            auto quote1 = line.find('"');
            auto quote2 = line.find('"', quote1 + 1);
            if (quote1 != std::string::npos && quote2 != std::string::npos) {
                std::string path = Trim(line.substr(quote1 + 1, quote2 - quote1 - 1));
                g_searchPaths.push_back(path);
            }
        }
    }

    if (g_gameDir.empty() || g_searchPaths.empty()) {
        std::cerr << "[FS] FS_Init failed to parse gameinfo.txt correctly." << std::endl;
        return false;
    }

    std::cout << "[FS] FS_Init successful.\n";
    std::cout << "[FS] Game Directory: " << g_gameDir << std::endl;
    for (const auto& path : g_searchPaths) {
        fs::path full = fs::path(g_gameDir) / path;
        std::cout << "[FS] Mount Path: " << full.string() << std::endl;
    }

    return true;
}

DLL_EXPORT std::string FS_ResolvePath(const std::string& relative_path) {
    return GetFileSystemInterface()->ResolvePath(relative_path);
}

DLL_EXPORT const std::string& FS_GetGameDir() {
    return g_gameDir;
}

DLL_EXPORT const std::vector<std::string>& FS_GetSearchPaths() {
    return g_searchPaths;
}

extern "C" DLL_EXPORT void FS_Shutdown() {
    // Clear internal state and release any resources
    g_gameDir.clear();
    g_searchPaths.clear();
    // Add other cleanup here if needed
}