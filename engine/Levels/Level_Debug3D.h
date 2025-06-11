#pragma once
#include "Level.h"

class Level_Debug3D : public Level {
public:
    virtual void Init() override;
    virtual void Update(float dt) override;
    virtual void Render() override;
    virtual void Unload() override;
};