#pragma once

#include "shaderapi/gpu_render_interface.h"
#include "mathlib/matrix4x4_f.h"
#include <SDL2/SDL.h>

// Access the global GPU render interface pointer
IGPURenderInterface* GetRenderInterface();

// Initialize the renderer with GPU interface and SDL window
bool Renderer_Init(IGPURenderInterface* gpuRender, SDL_Window* window);

// Per-frame render call
void Renderer_RenderFrame(const Matrix4x4_f& viewMatrix, const Matrix4x4_f& projMatrix, float totalTime);

// Shutdown and cleanup GPU renderer
void Renderer_Shutdown();