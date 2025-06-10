#include "LevelManager.h"
#include "Level.h"

std::unique_ptr<Level> LevelManager::currentLevel = nullptr;

void LevelManager::SetLevel(std::unique_ptr<Level> newLevel) {
    if (currentLevel) currentLevel->Unload();
    currentLevel = std::move(newLevel);
    currentLevel->Init();
}

void LevelManager::Update(float deltaTime) {
    if (currentLevel) currentLevel->Update(deltaTime);
}

void LevelManager::Render() {
    if (currentLevel) currentLevel->Render();
}

void LevelManager::Unload() {
    if (currentLevel) currentLevel->Unload();
    currentLevel.reset();
}