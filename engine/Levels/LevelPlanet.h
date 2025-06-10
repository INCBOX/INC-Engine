#pragma once
#include "LevelBase.h"
#include "Math.h"
#include <string>

class LevelPlanet : public LevelBase {
public:
    LevelPlanet(const std::string& name, const Vec3& pos, float radius, const Vec3& color);

    void Init() override;
    void Update(float dt) override;
    void Render(const float* view, const float* projection) override;
    void Unload() override;
    const char* GetName() const override;

    const Vec3& GetPosition() const; // ✅ Public accessor

private:
    std::string name;
    Vec3 position;
    float radius;
    Vec3 color;
    bool initialized = false;
};
