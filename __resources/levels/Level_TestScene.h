#pragma once
#include "Levels/Level.h"

class Level_TestScene : public Level {
public:
    void Init() override;
    void Update(float deltaTime) override;
    void Render() override;
    void Unload() override;

private:
    unsigned int vaoGround = 0;
    unsigned int vboGround = 0;
    unsigned int shaderProgram = 0;
};