#pragma once

#include <SDL.h> // forward declare SDL_Window if needed in future

class Renderer {
public:
    // Initialize OpenGL context and state.
    // Must be called after creating the SDL_GLContext and window.
    // Returns true on success, false otherwise.
    bool Init();

    // Render a frame, clears buffers and draws scene.
    // Parameters:
    // - width, height: current window or viewport dimensions.
    void RenderFrame(int width, int height);

    // Release all GPU resources and clean up.
    // Should be called before program exit or context destruction.
    void Shutdown();
};