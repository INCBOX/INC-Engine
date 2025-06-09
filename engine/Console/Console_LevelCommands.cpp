#include "Console.h"
#include "../Levels/LevelManager.h"
#include <iostream>

// Handles all level-related commands
bool HandleLevelCommands(const std::string& command) {
    if (command.rfind("loadlevel ", 0) == 0) {
        std::string levelName = command.substr(10);
        std::cout << "Loading level: " << levelName << std::endl;
        LevelManager::UnloadActiveLevel();
        if (!LevelManager::LoadLevelByName(levelName)) {
            std::cout << "Failed to load level: " << levelName << std::endl;
        }
        return true;
    } else if (command == "listlevels") {
        std::cout << "Available Levels:\n";
        for (const auto& name : LevelManager::GetAvailableLevels()) {
            std::cout << " - " << name << "\n";
        }
        return true;
    }

    return false; // Not a level command
}