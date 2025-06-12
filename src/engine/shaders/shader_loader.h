#pragma once
#include <string>

// Loads a vertex + fragment shader pair from file using FS_ResolvePath
// Returns OpenGL shader program ID, or 0 on failure
unsigned int LoadShaderFromFile(const std::string& vertexRelPath, const std::string& fragmentRelPath);
