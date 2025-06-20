#pragma once


#include <vector>
#include <memory>
#include <nlohmann/json.hpp>  // FOR JSON GEOMETRY
#include "shaderapi/geometry_shaderapi.h"
#include "mathlib/vector.h"
#include "mathlib/matrix4x4.h"

struct StaticMeshInstance {
    std::unique_ptr<IGeometry> mesh;
    Matrix4x4 transform;
};

void ClearStaticGeometry();
void LoadStaticGeometryFromMap(const nlohmann::json& mapData);
const std::vector<StaticMeshInstance>& GetStaticGeometry();