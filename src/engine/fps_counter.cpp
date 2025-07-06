#include "fps_counter.h"
#include <iostream>

FPSCounter::FPSCounter() : frameCount(0), fps(0) {
    lastTime = std::chrono::steady_clock::now();
}

void FPSCounter::FrameTick() {
    frameCount++;
    auto now = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed = now - lastTime;
    if (elapsed.count() >= 1.0) {
        fps = frameCount / elapsed.count();
        frameCount = 0;
        lastTime = now;
        std::cout << "[FPS] " << fps << std::endl;
    }
}

double FPSCounter::GetFPS() const {
    return fps;
}