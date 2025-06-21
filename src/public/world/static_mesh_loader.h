#pragma once


#include <vector>
#include <memory>
#include <nlohmann/json.hpp>  // FOR JSON GEOMETRY
#include "shaderapi/static_mesh_shaderapi.h"
#include "mathlib/vector3_f.h"
#include "mathlib/matrix4x4_f.h"

struct StaticMeshInstance {
    std::unique_ptr<IGeometry> mesh;
    Mat4_f transform;
};

void ClearStaticGeometry();
void LoadStaticGeometryFromMap(const nlohmann::json& mapData);
const std::vector<StaticMeshInstance>& GetStaticGeometry();