#pragma once

class LevelBase {
public:
    virtual ~LevelBase() = default;
    virtual void Init() = 0;
    virtual void Update(float dt) = 0;
    virtual void Render(const float* view, const float* projection) = 0;
    virtual void Unload() = 0;
    virtual const char* GetName() const = 0;
};
