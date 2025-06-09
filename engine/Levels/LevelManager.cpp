#include "Core/RuntimeDataPath.h"
#include "LevelManager.h"
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
namespace fs = std::filesystem;

// Internal state
static std::string activeLevel;
static std::vector<std::string> availableLevels;

// Initialize level manager systems
void LevelManager::Init() {
    std::cout << "[LevelManager] Initialized\n";
    DiscoverLevels();
}

// Scan directory for level files
void LevelManager::DiscoverLevels() {
    availableLevels.clear();

    const std::string levelDir = DataPath("levels/");
    try {
        for (const auto& entry : fs::directory_iterator(levelDir)) {
            if (entry.is_regular_file()) {
                auto path = entry.path();
                std::string ext = path.extension().string();
                if (ext == ".cpp" || ext == ".lvl") {
                    std::string name = path.stem().string();
                    if (name.rfind("TestLevel", 0) == 0 || name == "MarsLevel_1") {
                        availableLevels.push_back(name);
                    }
                }
            }
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "[LevelManager] Error reading level directory: " << e.what() << std::endl;
    }
}

// Load a level by its name
bool LevelManager::LoadLevelByName(const std::string& name) {
    std::cout << "[LevelManager] Loading level: " << name << std::endl;
    activeLevel = name;

    // TODO: Call matching load function, e.g. InitMarsLevel() etc.
    return true;
}

void LevelManager::RenderActiveLevel(const float* view, const float* projection) {
    if (!activeLevel.empty()) {
        // TODO: draw actual level objects
    }
}

void LevelManager::UnloadActiveLevel() {
    std::cout << "[LevelManager] Unloading level: " << activeLevel << std::endl;
    activeLevel.clear();
}

std::vector<std::string> LevelManager::GetAvailableLevels() {
    return availableLevels;
}