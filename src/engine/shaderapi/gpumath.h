#pragma once

#include "mathlib/matrix.h" 	// CPU-side Matrix class
#include "mathlib/vector.h"   	// CPU-side Vec3 class

// GPU-aligned float4x4 matrix — matches GLSL std140 layout
struct GpuMat4
{
    float m[16]; // column-major order for GLSL mat4
};

// GPU vec3 (with padding to 16 bytes for std140 compatibility)
struct GpuVec3
{
    float x, y, z, pad;
};

// Combined transform block (for uniform buffer upload)
struct GpuTransformData
{
    GpuMat4 model;
    GpuMat4 view;
    GpuMat4 proj;
};

// Converts your CPU-side Matrix to GPU-ready flat matrix
inline GpuMat4 ConvertToGpu(const Matrix& mat)
{
    GpuMat4 result;
    for (int col = 0; col < 4; ++col)
    {
        for (int row = 0; row < 4; ++row)
        {
            result.m[col * 4 + row] = mat.m[col][row];  // column-major layout
        }
    }
    return result;
}

// Converts Vec3 to padded GpuVec3
inline GpuVec3 ConvertToGpu(const Vec3& vec)
{
    return { vec.x, vec.y, vec.z, 0.0f };
}