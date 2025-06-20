#include "world/geometry_loader.h"
#include "world/mesh_primitives.h"
#include "mathlib/math_constants.h"
#include "shaderapi/shaderapi.h"
#include <nlohmann/json.hpp>
#include "engine_log.h"

static std::vector<StaticMeshInstance> g_StaticMeshes;

void ClearStaticGeometry() {
    g_StaticMeshes.clear();
}

void LoadStaticGeometryFromMap(const nlohmann::json& mapData) {
    EngineLog("[LoadStaticGeometryFromMap] Clearing previous static geometry.");
    ClearStaticGeometry();

    if (!mapData.contains("entities")) {
        EngineLog("[LoadStaticGeometryFromMap] No 'entities' found in map data.");
        return;
    }

    using namespace geometry;  // For Create*Mesh calls

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