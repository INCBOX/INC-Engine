#pragma once

class Level {
public:
    virtual ~Level() = default;

    virtual void Init() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void Render() = 0;
    virtual void Shutdown() = 0;  // ✅ Add this if it was missing
};
