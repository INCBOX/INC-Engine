#include "shaderapi/shaderapi.h" 		// access to g_pShaderAPI
#include "shaderapi/ishaderapi.h"      	// interface for ShaderAPICore
#include "shaderapi/imesh.h"          	// IMesh interface
#include "mathlib/matrix.h"           	// for Matrix::Translation
#include "world/geometry_loader.h"
#include <nlohmann/json.hpp>
#include <iostream>
#include "engine_log.h"

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
    EngineLog("[LoadStaticGeometryFromMap] Clearing previous static geometry.");
    ClearStaticGeometry();

    if (!mapData.contains("entities")) {
        EngineLog("[LoadStaticGeometryFromMap] No 'entities' found in map data.");
        return;
    }

    for (const auto& ent : mapData["entities"]) {
        if (ent.value("classname", "") != "static_geometry")
            continue;

        auto origin = ent.value("origin", std::vector<float>{0, 0, 0});
        Vector position(origin[0], origin[1], origin[2]);

        if (!ent.contains("geometry")) {
            EngineLog("[LoadStaticGeometryFromMap] Entity at position (%.2f, %.2f, %.2f) missing 'geometry' key.", position.x, position.y, position.z);
            continue;
        }

        const auto& geo = ent["geometry"];
        std::string type = geo.value("type", "");

        std::vector<float> verts;
        std::vector<unsigned int> indices;

        if (type == "cube") {
            auto size = geo.value("size", std::vector<float>{1, 1, 1});
            EngineLog("[LoadStaticGeometryFromMap] Creating cube at (%.2f, %.2f, %.2f) with size (%.2f, %.2f, %.2f).",
                      position.x, position.y, position.z, size[0], size[1], size[2]);
            CreateCubeMesh(verts, indices, Vector(size[0], size[1], size[2]));
        } else if (type == "plane") {
            auto size = geo.value("size", std::vector<float>{1, 1});
            EngineLog("[LoadStaticGeometryFromMap] Creating plane at (%.2f, %.2f, %.2f) with size (%.2f, %.2f).",
                      position.x, position.y, position.z, size[0], size[1]);
            CreatePlaneMesh(verts, indices, Vector(size[0], 0.0f, size[1]));
        } else if (type == "sphere") {
            float radius = geo.value("radius", 1.0f);
            int slices = geo.value("slices", 32);
            int stacks = geo.value("stacks", 16);
            EngineLog("[LoadStaticGeometryFromMap] Creating sphere at (%.2f, %.2f, %.2f) with radius %.2f, slices %d, stacks %d.",
                      position.x, position.y, position.z, radius, slices, stacks);
            CreateSphereMesh(verts, indices, radius, slices, stacks);
        } else {
            EngineLog("[LoadStaticGeometryFromMap] Unknown geometry type: '%s' at position (%.2f, %.2f, %.2f).",
                      type.c_str(), position.x, position.y, position.z);
            continue;
        }

        StaticMeshInstance instance;
        instance.mesh.reset(g_pShaderAPI->CreateMesh());

        try {
            instance.mesh->Upload(verts, indices);
        } catch (const std::exception& e) {
            EngineLog("[LoadStaticGeometryFromMap] Exception during mesh upload: %s", e.what());
            continue;
        } catch (...) {
            EngineLog("[LoadStaticGeometryFromMap] Unknown exception during mesh upload.");
            continue;
        }

        instance.transform = Matrix::Translation(position);

        g_StaticMeshes.push_back(std::move(instance));
        EngineLog("[LoadStaticGeometryFromMap] Mesh added. Total static meshes: %zu", g_StaticMeshes.size());
    }
}

const std::vector<StaticMeshInstance>& GetStaticGeometry() {
    return g_StaticMeshes;
}