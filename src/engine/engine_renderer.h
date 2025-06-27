#pragma once
#include "shaderapi/gpu_render_interface.h"
#include "mathlib/matrix4x4_f.h"

#include <SDL.h>

// Initialize the renderer module with the GPU interface pointer
void Renderer_Init(IGPURenderInterface* gpuRender, SDL_Window* window);

// Called every frame for rendering
void Renderer_RenderFrame(const Matrix4x4_f& viewMatrix, const Matrix4x4_f& projMatrix, float totalTime);

// Shutdown renderer
void Renderer_Shutdown();