#pragma once
#include "Level.h"
#include "mathlib/Vec3.h"
#include <vector>
#include <string>

struct Planet {
    std::string name;
    mathlib::Vec3 position;
    float radius;
    std::string texturePath;
};

class SphereMesh {
public:
    void Init();
    void Render();
    void Cleanup();

private:
    unsigned int VAO = 0, VBO = 0, EBO = 0;
    int indexCount = 0;
};

class Level_Space : public Level {
public:
    void Init() override;
    void Update(float deltaTime) override;
    void Render() override;
    void Unload() override;

private:
    std::vector<Planet> planets;
    SphereMesh sphereMesh;
    unsigned int shaderID = 0;
};