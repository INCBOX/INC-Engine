#pragma once

#include <string>

// Loads a texture from "materials/<name>.png" and returns OpenGL texture ID
unsigned int LoadTextureFromIMT(const std::string& basetexturePath);
