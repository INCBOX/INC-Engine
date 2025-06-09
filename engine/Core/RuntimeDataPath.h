#pragma once
#include <string>

// Centralized root for all runtime-loaded files
inline const std::string RUNTIME_DATA_ROOT = "gamedata/";

// Joins root + relative path
inline std::string DataPath(const std::string& relative) {
    return RUNTIME_DATA_ROOT + relative;
}