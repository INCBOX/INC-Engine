#pragma once
#include <chrono>

class FPSCounter {
public:
    FPSCounter();

    // Call this once per frame
    void FrameTick();

    // Get last calculated FPS
    double GetFPS() const;

private:
    int frameCount;
    double fps;
    std::chrono::steady_clock::time_point lastTime;
};