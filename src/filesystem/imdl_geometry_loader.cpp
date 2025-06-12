// imdl_loader.cpp - Loads and renders static .imdl files

#include "imdl_loader.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <glad/glad.h>
#include <vector>
#include <iostream>

struct IMDLModel {
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;
    GLsizei indexCount = 0;
    std::string material;
};

bool LoadIMDL(const std::string& path, IMDLModel& outModel) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "[IMDL] Failed to open " << path << std::endl;
        return false;
    }

    nlohmann::json json;
    file >> json;

    if (!json.contains("vertices") || !json.contains("indices")) {
        std::cerr << "[IMDL] Missing vertices or indices in " << path << std::endl;
        return false;
    }

    const auto& verts = json["vertices"];
    const auto& norms = json["normals"];
    const auto& uvs = json["uvs"];
    const auto& indices = json["indices"];

    std::vector<float> vertexData;
    for (size_t i = 0; i < verts.size(); ++i) {
        auto v = verts[i];
        auto n = norms[i];
        auto uv = uvs[i];
        vertexData.insert(vertexData.end(), {v[0], v[1], v[2], n[0], n[1], n[2], uv[0], uv[1]});
    }

    std::vector<unsigned int> indexData(indices.begin(), indices.end());

    GLuint vao, vbo, ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexData.size() * sizeof(unsigned int), indexData.data(), GL_STATIC_DRAW);

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // UV
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    outModel.vao = vao;
    outModel.vbo = vbo;
    outModel.ebo = ebo;
    outModel.indexCount = static_cast<GLsizei>(indexData.size());
    outModel.material = json.value("material", "");

    std::cout << "[IMDL] Loaded model: " << path << " with " << outModel.indexCount << " indices." << std::endl;
    return true;
}

void DrawIMDL(const IMDLModel& model) {
    glBindVertexArray(model.vao);
    glDrawElements(GL_TRIANGLES, model.indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
