#pragma once

#include "mathlib/matrix4x4_f.h" 	// CPU-side Matrix class
#include "mathlib/vector3_f.h"   	// CPU-side Vector3_f class

// GPU-aligned float4x4 matrix — matches GLSL std140 layout
struct GpuMatrix4x4_f
{
    float m[16]; // column-major order for GLSL mat4
};

// GPU vec3 (with padding to 16 bytes for std140 compatibility)
struct GpuVector3_f
{
    float x, y, z, pad;
};

// Combined transform block (for uniform buffer upload)
struct GpuTransformData
{
    GpuMatrix4x4_f model;
    GpuMatrix4x4_f view;
    GpuMatrix4x4_f proj;
};

// Converts your CPU-side Matrix to GPU-ready flat matrix
inline GpuMatrix4x4_f ConvertToGpu(const Matrix4x4_f& mat)
{
    GpuMatrix4x4_f result;
    for (int col = 0; col < 4; ++col)
    {
        for (int row = 0; row < 4; ++row)
        {
            result.m[col * 4 + row] = mat.m[col][row];  // column-major layout
        }
    }
    return result;
}

// Converts Vector3_f to padded GpuVector3_f
inline GpuVector3_f ConvertToGpu(const Vector3_f& vec)
{
    return { vec.x, vec.y, vec.z, 0.0f };
}