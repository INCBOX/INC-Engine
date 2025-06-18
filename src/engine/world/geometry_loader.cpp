#include "shaderapi/shaderapi.h" 		// access to g_pShaderAPI
#include "shaderapi/ishaderapi.h"      	// interface for ShaderAPICore
#include "shaderapi/imesh.h"          	// IMesh interface
#include "mathlib/matrix.h"           	// for Matrix::Translation
#include "world/geometry_loader.h"
#include <nlohmann/json.hpp>
#include <iostream>

// Static storage of all loaded static meshes
static std::vector<StaticMeshInstance> g_StaticMeshes;

void ClearStaticGeometry() {
    g_StaticMeshes.clear();
}

// Create a simple cube mesh (centered at origin)
void CreateCubeMesh(std::vector<float>& verts, std::vector<unsigned int>& indices, const Vector& size) {
    float sx = size.x * 0.5f, sy = size.y * 0.5f, sz = size.z * 0.5f;

    float cubeVerts[] = {
        -sx, -sy, -sz,  sx, -sy, -sz,  sx,  sy, -sz,  -sx,  sy, -sz,
        -sx, -sy,  sz,  sx, -sy,  sz,  sx,  sy,  sz,  -sx,  sy,  sz
    };

    unsigned int cubeIndices[] = {
        0,1,2, 2,3,0,
        1,5,6, 6,2,1,
        5,4,7, 7,6,5,
        4,0,3, 3,7,4,
        3,2,6, 6,7,3,
        4,5,1, 1,0,4
    };

    verts.assign(cubeVerts, cubeVerts + 8 * 3);
    indices.assign(cubeIndices, cubeIndices + 36);
}

// Create a simple plane mesh on the XZ plane, centered at origin
void CreatePlaneMesh(std::vector<float>& verts, std::vector<unsigned int>& indices, const Vector& size) {
    float sx = size.x * 0.5f, sz = size.z * 0.5f;

    float planeVerts[] = {
        -sx, 0, -sz,  sx, 0, -sz,  sx, 0, sz,  -sx, 0, sz
    };

    unsigned int planeIndices[] = {
        0,1,2, 2,3,0
    };

    verts.assign(planeVerts, planeVerts + 12);
    indices.assign(planeIndices, planeIndices + 6);
}

void LoadStaticGeometryFromMap(const nlohmann::json& mapData) {
    ClearStaticGeometry();

    if (!mapData.contains("entities")) return;

    for (const auto& ent : mapData["entities"]) {
        if (ent.value("classname", "") != "static_geometry") continue;

        auto origin = ent.value("origin", std::vector<float>{0, 0, 0});
        Vector position(origin[0], origin[1], origin[2]);

        if (!ent.contains("geometry")) continue;
        const auto& geo = ent["geometry"];
        std::string type = geo.value("type", "");

        std::vector<float> verts;
        std::vector<unsigned int> indices;

        if (type == "cube") {
            auto size = geo.value("size", std::vector<float>{1, 1, 1});
            CreateCubeMesh(verts, indices, Vector(size[0], size[1], size[2]));
        } else if (type == "plane") {
            auto size = geo.value("size", std::vector<float>{1, 1});
            // Note: passing size as Vector(x, 0, z) for plane mesh on XZ plane
            CreatePlaneMesh(verts, indices, Vector(size[0], 0.0f, size[1]));
        } else {
            std::cerr << "[GeoLoader] Unknown geometry type: " << type << "\n";
            continue;
        }

        StaticMeshInstance instance;
        instance.mesh.reset(g_pShaderAPI->CreateMesh());
        instance.mesh->Upload(verts, indices);

        // Create translation matrix from position
        instance.transform = Matrix::Translation(position);

        g_StaticMeshes.push_back(std::move(instance));
    }
}

const std::vector<StaticMeshInstance>& GetStaticGeometry() {
    return g_StaticMeshes;
}