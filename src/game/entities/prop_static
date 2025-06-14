// Immobile, baked into level — used for walls, crates, pipes, etc.

#include "prop_static.h"
#include <iostream>

bool PropStatic::LoadFromEntity(const nlohmann::json& ent) {
    if (!ent.contains("model") || !ent.contains("origin")) return false;

    std::string path = ent["model"];
    if (!LoadIMDL(path, model)) {
        std::cerr << "[PropStatic] Failed to load model: " << path << "\n";
        return false;
    }

    const auto& o = ent["origin"];
    origin = Vec3(o[0], o[1], o[2]);
    return true;
}

void PropStatic::Draw(const Mat4& view, const Mat4& proj) {
    Mat4 model = Mat4::Translation(origin);
    Mat4 mvp = proj * view * model;
    DrawIMDL(modelData, mvp);  // new overload
}

