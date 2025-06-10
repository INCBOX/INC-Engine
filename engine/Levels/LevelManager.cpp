#include "LevelManager.h"
#include "LevelBase.h"
#include "LevelPlanet.h"
#include "LevelBase.h"
#include "Math.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <memory>
#include <cmath>
#include <filesystem>

using json = nlohmann::json;

static std::unordered_map<std::string, std::unique_ptr<LevelBase>> allLevels;
static std::unordered_map<std::string, bool> activeStatus;

void LevelManager::Init(const std::string& jsonPath) {
	
	std::ifstream file(jsonPath);
	if (!file.is_open()) {
		std::cerr << "[LevelManager] Failed to open: " << jsonPath << "\\n";
		return;
	}
	
	json planetData;
	file >> planetData;
	
	std::cout << "[LevelManager] Init from: " << jsonPath << std::endl;
	std::cout << "[LevelManager] Total planets: " << planetData.size() << std::endl;

    for (const auto& planet : planetData) {
        std::string name = planet.value("name", "unknown");
        std::string levelKey = planet.value("level", name);
        auto pos = planet.value("position", std::vector<float>{0, 0, 0});
        auto color = planet.value("color", std::vector<float>{1, 1, 1});
        float radius = planet.value("radius", 100.0f);

        Vec3 position(pos[0], pos[1], pos[2]);
        Vec3 rgb(color[0], color[1], color[2]);

        std::unique_ptr<LevelBase> level = std::make_unique<LevelPlanet>(levelKey, position, radius, rgb);
        allLevels[levelKey] = std::move(level);
        activeStatus[levelKey] = false;

        std::cout << "[LevelManager] Registered level: " << levelKey << "\n";
    }
}

void LevelManager::Tick(float deltaTime, const float* view, const float* projection, const float* playerPos) {
    for (auto& [name, level] : allLevels) {
        float dist = 999999.0f;
        if (playerPos) {
            const Vec3 pos(playerPos[0], playerPos[1], playerPos[2]);
            const Vec3 levelPos = static_cast<LevelPlanet*>(level.get())->GetPosition();
            dist = (pos - levelPos).length();
        }

        if (dist < 10000.0f && !activeStatus[name]) {
            level->Init();
            activeStatus[name] = true;
            std::cout << "[LevelManager] Activated: " << name << "\n";
        } else if (dist > 15000.0f && activeStatus[name]) {
            level->Unload();
            activeStatus[name] = false;
            std::cout << "[LevelManager] Deactivated: " << name << "\n";
        }

        if (activeStatus[name]) {
            level->Update(deltaTime);
            level->Render(view, projection);
        }
    }
}

void LevelManager::LoadLevelByName(const std::string& name) {
    if (allLevels.count(name) && !activeStatus[name]) {
        allLevels[name]->Init();
        activeStatus[name] = true;
    }
}

void LevelManager::UnloadLevelByName(const std::string& name) {
    if (allLevels.count(name) && activeStatus[name]) {
        allLevels[name]->Unload();
        activeStatus[name] = false;
    }
}

bool LevelManager::IsLevelActive(const std::string& name) {
    return activeStatus[name];
}

std::vector<std::string> LevelManager::GetActiveLevelNames() {
    std::vector<std::string> names;
    for (const auto& [name, active] : activeStatus) {
        if (active) names.push_back(name);
    }
    return names;
}
