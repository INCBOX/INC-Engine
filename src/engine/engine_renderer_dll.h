#pragma once
#include <SDL2/SDL.h>

// Load shaderapi.dll, create GPU interface, and initialize it
bool RendererDLL_LoadAndInit(SDL_Window* window);

// Unload shaderapi.dll and cleanup
void RendererDLL_Unload();