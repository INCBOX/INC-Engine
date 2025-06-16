#pragma once

#include <vector>
#include <memory>
#include <nlohmann/json.hpp>  // FOR JSON GEOMETRY
#include "shaderapi/mesh.h"
#include "mathlib/vector.h"
#include "mathlib/matrix.h"

struct StaticMeshInstance {
    std::unique_ptr<Mesh> mesh;
    Matrix transform;
};

void ClearStaticGeometry();
void LoadStaticGeometryFromMap(const nlohmann::json& mapData);
const std::vector<StaticMeshInstance>& GetStaticGeometry();
