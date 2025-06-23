#pragma once

#include <vector>
#include "mathlib/vector3_f.h"

namespace geometry {

// Fills verts and indices with cube mesh data (centered at origin)
void CreateCubeMesh(std::vector<float>& verts, std::vector<unsigned int>& indices, const Vector3_f& size);

// Fills verts and indices with a flat XZ plane at Y = 0
void CreatePlaneMesh(std::vector<float>& verts, std::vector<unsigned int>& indices, const Vector3_f& size);

// Fills verts and indices with a UV sphere
void CreateSphereMesh(std::vector<float>& verts, std::vector<unsigned int>& indices, float radius, int slices, int stacks);

} // namespace geometry