// filesystem_stdio.cpp - updated for Source-style directory structure
#include "filesystem_stdio.h"

#include <vector> // added because contains vector stuff
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;

static std::string g_gameDir;
static std::vector<std::string> g_searchPaths;

static std::string Trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    size_t last = str.find_last_not_of(" \t\n\r");
    return (first == std::string::npos) ? "" : str.substr(first, last - first + 1);
}

DLL_EXPORT bool FS_Init(const std::string& gameinfo_path) {
    g_gameDir = fs::absolute(fs::path(gameinfo_path)).parent_path().string();

    std::ifstream file(gameinfo_path);
    if (!file.is_open()) {
        std::cerr << "[FS] Failed to open gameinfo.txt: " << gameinfo_path << std::endl;
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
        std::cerr << "[FS] Failed to parse gameinfo.txt correctly." << std::endl;
        return false;
    }

    std::cout << "[FS] Game Directory: " << g_gameDir << std::endl;
    for (const auto& path : g_searchPaths) {
        fs::path full = fs::path(g_gameDir) / path;
        std::cout << "[FS] Mount Path: " << full.string() << std::endl;
    }

    return true;
}

const std::string& FS_GetGameDir() {
    return g_gameDir;
}

const std::vector<std::string>& FS_GetSearchPaths() {
    return g_searchPaths;
}

std::string FS_ResolvePath(const std::string& relative_path) {
    std::cout << "[FS] Resolving: " << relative_path << "\n";
    for (const auto& base : g_searchPaths) {
        fs::path full = fs::path(g_gameDir) / base / relative_path;
        std::cout << "[FS] Checking: " << full << "\n";
        if (fs::exists(full)) {
            std::cout << "[FS] Found: " << full << "\n";
            return full.string();
        }
    }
    std::cout << "[FS] Failed to resolve: " << relative_path << "\n";
    return "";
}