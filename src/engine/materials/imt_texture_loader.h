
#pragma once

#include <string>
#include <cstdint>

// Loads a DDS texture from "materials/<name>.dds" and returns a GPU texture ID.
// Uses IFileSystem and ShaderAPI interfaces behind the scenes.
uint32_t LoadTextureFromIMT(const std::string& basetexturePath);
