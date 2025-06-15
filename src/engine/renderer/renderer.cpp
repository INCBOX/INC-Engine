// src/engine/renderer/renderer.cpp
#include "renderer/renderer.h"
#include "shaders/gl_rendersystem.h" // For now, we directly create GL system
#include <iostream>
#include <SDL.h> // Include SDL for SDL_GL_CreateContext and SDL_GL_SwapWindow (though not directly used here now)

// No more Shader sources or RendererInternal OpenGL-specific state here!

Renderer::Renderer() : m_activeRenderSystem(nullptr) {}

Renderer::~Renderer() {
    // Shutdown is responsible for deleting m_activeRenderSystem
}

bool Renderer::Init(SDL_Window* window) {
    // This is where you would potentially load a DLL for DX/Vulkan/OpenGL
    // For now, we directly instantiate RenderSystemGL
    m_activeRenderSystem = new RenderSystemGL(); // <--- Instantiate the OpenGL implementation

    if (!m_activeRenderSystem) {
        std::cerr << "[Renderer] Failed to create a render system instance.\n";
        return false;
    }

    // Pass the SDL window handle to the specific render system for context creation
    // The width and height could be queried from the window or passed from engine.cpp
    int width, height;
    SDL_GetWindowSize(window, &width, &height);

    if (!m_activeRenderSystem->Init(window, width, height)) {
        std::cerr << "[Renderer] Active render system initialization failed.\n";
        delete m_activeRenderSystem;
        m_activeRenderSystem = nullptr;
        return false;
    }

    std::cout << "[Renderer] Init successful, using active render system.\n";
    return true;
}

void Renderer::BeginFrame() {
    if (m_activeRenderSystem) {
        m_activeRenderSystem->BeginFrame();
    }
}

void Renderer::RenderFrame(int width, int height) {
    if (m_activeRenderSystem) {
        // No triangle or vertex submission here!

        // The render system just renders whatever was submitted earlier by the game or scene.

        m_activeRenderSystem->RenderFrame(width, height);
    }
}

void Renderer::EndFrame() { // <--- REMOVED SDL_Window* parameter
    if (m_activeRenderSystem) {
        m_activeRenderSystem->EndFrame();
        // SDL_GL_SwapWindow is now handled by the specific RenderSystemGL::EndFrame()
        // No direct SDL_GL_SwapWindow call here.
    }
}

void Renderer::Shutdown() {
    if (m_activeRenderSystem) {
        m_activeRenderSystem->Shutdown(); // Call shutdown on the active render system
        delete m_activeRenderSystem; // Delete the instance
        m_activeRenderSystem = nullptr;
    }
    std::cout << "[Renderer] Shutdown complete.\n";
}

void Renderer::DrawTriangle() {
    // This is a placeholder for how game logic might request drawing.
    // It would ultimately call SubmitObject on the active render system.
}