#pragma once
#include <memory>
class Level;

class LevelManager {
public:
    static void SetLevel(std::unique_ptr<Level> newLevel);
    static void Update(float deltaTime);
    static void Render();
    static void Unload();

private:
    static std::unique_ptr<Level> currentLevel;
};