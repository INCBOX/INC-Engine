#pragma once
#include <string>
#include <vector>
#include <cstdint>

// IMDL_GPU = raw CPU-side geometry
struct IMDL_CPU {
    std::vector<float> vertexData;       // Interleaved vertex buffer (pos, norm, uv)
    std::vector<uint32_t> indexData;     // Index buffer
    std::string material;

    float position[3] = {0.0f, 0.0f, 0.0f};      // World-space position
};
