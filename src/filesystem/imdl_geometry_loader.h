// --- filesystem/imdl_geometry_loader.h ---
#pragma once

#include <string>
#include <vector>
#include <cstdint>

struct IMDLGeometry {
    std::string material;
    std::vector<float> vertexData; // xyz, normal, uv (8 floats per vertex)
    std::vector<uint32_t> indexData;
};

bool ParseIMDLFile(const std::string& path, IMDLGeometry& out);
