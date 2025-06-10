#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

class LevelBase;

class LevelManager {
public:
    static void Init(const std::string& jsonPath);
    static void Tick(float deltaTime, const float* view, const float* projection, const float* playerPos);

    static void RegisterLevel(std::unique_ptr<LevelBase> level);
    static void RegisterAllLevels();
    static void LoadLevelByName(const std::string& name);
    static void UnloadLevelByName(const std::string& name);
    static bool IsLevelActive(const std::string& name);
    static std::vector<std::string> GetActiveLevelNames();
};
