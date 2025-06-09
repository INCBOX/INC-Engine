#ifndef LEVEL_MANAGER_H
#define LEVEL_MANAGER_H

#include <string>
#include <vector>
#include <unordered_map>

class LevelManager {
public:
    static void Init();
    static bool LoadLevelByName(const std::string& name);
    static void RenderActiveLevel(const float* view, const float* projection);
    static void UnloadActiveLevel();
    static std::vector<std::string> GetAvailableLevels();

private:
    static void DiscoverLevels(); // new
};

#endif // LEVEL_MANAGER_H
