#pragma once
#include "Levels/Level.h"

class Level_TestScene : public Level {
public:
    void Init() override;
    void Update(float deltaTime) override;
    void Render() override;
	void Unload() override;  // LEVEL OVERRUN
};