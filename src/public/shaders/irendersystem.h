// src/public/shaders/irendersystem.h
#pragma once

#include <cstddef> // <--- ADD THIS for size_t
#include <vector>  // <--- ADD THIS for std::vector in RenderSystemGL, though it's technically in gl_rendersystem.h now, good practice for common types
// No, actually, it's better to keep <vector> in gl_rendersystem.h if only gl_rendersystem.h uses std::vector.
// However, RenderableObjectData uses size_t, which is why <cstddef> is crucial.

struct SDL_Window; // Forward declaration for SDL_Window if needed by implementations

// Generic representation of a 3D object for rendering
// This could be expanded significantly later for materials, textures, etc.
struct RenderableObjectData {
    // Example: simple vertex data for now.
    // In a real engine, this would be more complex (indices, vertex format description, etc.)
    float* vertices;
    size_t vertexCount; // Number of floats (e.g., if 3 floats per vertex, total floats)
    // Could also have:
    // unsigned int* indices;
    // size_t indexCount;
    // Material* material;
    // Matrix transform; // World transform for the object
};

// Interface for any specific rendering API (OpenGL, Vulkan, DirectX)
class IRenderSystem {
public:
    virtual ~IRenderSystem() = default;

    // Initializes the specific rendering API (OpenGL, Vulkan, DX)
    // windowHandle is void* to keep it generic, but expected to be SDL_Window*
    virtual bool Init(void* windowHandle, int width, int height) = 0;

    // Called once per frame before any drawing commands
    virtual void BeginFrame() = 0;

    // Main rendering logic, draws the scene
    // Dimensions needed for viewport
    virtual void RenderFrame(int width, int height) = 0;

    // Submits an object to be rendered (implementation details vary per API)
    virtual void SubmitObject(const RenderableObjectData& object) = 0;

    // Called once per frame after all drawing commands, typically swaps buffers
    virtual void EndFrame() = 0; // <--- NO window parameter here in the interface

    // Cleans up the rendering API resources
    virtual void Shutdown() = 0;

    // Handles window resize events
    virtual void OnResize(int width, int height) = 0;
};