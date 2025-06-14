// Loads and renders static .imdl files

#include "imdl_geometry.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>

bool ParseIMDLFile(const std::string& path, IMDL_CPU& out) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "[IMDL Loader] Failed to open " << path << "\n";
        return false;
    }

    nlohmann::json json;
    try {
        file >> json;
    } catch (const std::exception& e) {
        std::cerr << "[IMDL Loader] JSON parse error: " << e.what() << "\n";
        return false;
    }

    out.material = json.value("material", "");

    if (!json.contains("vertices") || !json.contains("triangles")) {
        std::cerr << "[IMDL Loader] Missing required fields\n";
        return false;
    }

    auto& verts = json["vertices"];
    auto& tris = json["triangles"];

    if (!verts.is_array() || !tris.is_array()) {
        std::cerr << "[IMDL Loader] Invalid data format\n";
        return false;
    }

    out.vertexData.clear();
    for (const auto& v : verts) {
        if (v.size() != 8) {
            std::cerr << "[IMDL Loader] Invalid vertex entry (must have 8 floats)\n";
            return false;
        }
        for (int i = 0; i < 8; ++i)
            out.vertexData.push_back(v[i]);
    }

    out.indexData.clear();
    for (const auto& tri : tris) {
        if (tri.size() != 3) {
            std::cerr << "[IMDL Loader] Invalid triangle entry (must have 3 indices)\n";
            return false;
        }
        for (int i = 0; i < 3; ++i)
            out.indexData.push_back(tri[i]);
    }

    return true;
}