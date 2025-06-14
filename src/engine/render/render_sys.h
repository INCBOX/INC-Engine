#pragma once

#include <nlohmann/json_fwd.hpp>  // Forward declare only the JSON type for faster build times

//-----------------------------------------------------------------------------
// Render Subsystem (engine.dll)
//
// This interface is responsible for rendering all world geometry and models,
// driven by data parsed from JSON maps.
//
// Modules: engine.dll
// Depends on: ShaderAPI, IMDL loader, Material system
//-----------------------------------------------------------------------------

// Initializes the render system (GL/DX/Vulkan) and backend resources.
void Render_Init();

// Loads prop_static models from JSON map and prepares them for rendering.
// This parses "model", "material", and "origin" fields in each entity.
void Render_LoadMap(const nlohmann::json& mapData);

// Called every frame by the engine. Renders all loaded models.
void RenderFrame(int width, int height);

// Cleans up renderer and releases all GPU-side resources.
void Render_Shutdown();
