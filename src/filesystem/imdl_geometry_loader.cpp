// Loads and renders static .imdl files

#include "imdl_geometry_loader.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>

bool ParseIMDLFile(const std::string& path, IMDLGeometry& out) {
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

    auto vertices = json["vertices"];
    auto normals = json["normals"];
    auto uvs = json["uvs"];
    auto indices = json["indices"];

    if (vertices.size() != normals.size() || vertices.size() / 3 != uvs.size() / 2) {
        std::cerr << "[IMDL Loader] Vertex attribute size mismatch\n";
        return false;
    }

    size_t vertexCount = vertices.size() / 3;
    out.vertexData.resize(vertexCount * 8);

    for (size_t i = 0; i < vertexCount; ++i) {
        out.vertexData[i * 8 + 0] = vertices[i * 3 + 0];
        out.vertexData[i * 8 + 1] = vertices[i * 3 + 1];
        out.vertexData[i * 8 + 2] = vertices[i * 3 + 2];
        out.vertexData[i * 8 + 3] = normals[i * 3 + 0];
        out.vertexData[i * 8 + 4] = normals[i * 3 + 1];
        out.vertexData[i * 8 + 5] = normals[i * 3 + 2];
        out.vertexData[i * 8 + 6] = uvs[i * 2 + 0];
        out.vertexData[i * 8 + 7] = uvs[i * 2 + 1];
    }

    out.indexData.assign(indices.begin(), indices.end());
    return true;
}