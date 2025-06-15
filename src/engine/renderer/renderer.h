// renderer/renderer.h
#pragma once

#include "shaders/irendersystem.h" // Include the interface
// Forward declaration for SDL_Window if not already there
struct SDL_Window;

class Renderer {
public:
    Renderer(); // Constructor
    ~Renderer(); // Destructor

    bool Init(SDL_Window* window);
    void BeginFrame();
    void RenderFrame(int width, int height);
    void EndFrame(); // <--- REMOVED SDL_Window* parameter
    void Shutdown();

    // High-level rendering commands that delegate to the actual API
    void DrawTriangle();

private:
    IRenderSystem* m_activeRenderSystem = nullptr;
};