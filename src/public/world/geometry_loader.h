#pragma once


#include <vector>
#include <memory>
#include <nlohmann/json.hpp>  // FOR JSON GEOMETRY
#include "shaderapi/imesh.h"
#include "mathlib/vector.h"
#include "mathlib/matrix.h"

struct StaticMeshInstance {
    std::unique_ptr<IMesh> mesh;
    Matrix transform;
};

void ClearStaticGeometry();
void LoadStaticGeometryFromMap(const nlohmann::json& mapData);
const std::vector<StaticMeshInstance>& GetStaticGeometry();

// Sphere mesh creation for future use:
void CreateSphereMesh(std::vector<float>& verts, std::vector<unsigned int>& indices, float radius, int slices, int stacks);