// --- engine/models/imdl_model_runtime.cpp ---
#include "imdl_model_runtime.h"
#include <glad/glad.h>
#include <iostream>

bool UploadIMDLToGPU(const IMDLGeometry& data, IMDLModel& out) {
    glGenVertexArrays(1, &out.vao);
    glGenBuffers(1, &out.vbo);
    glGenBuffers(1, &out.ebo);

    glBindVertexArray(out.vao);

    glBindBuffer(GL_ARRAY_BUFFER, out.vbo);
    glBufferData(GL_ARRAY_BUFFER, data.vertexData.size() * sizeof(float), data.vertexData.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, out.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.indexData.size() * sizeof(uint32_t), data.indexData.data(), GL_STATIC_DRAW);

    // Vertex format: vec3 pos, vec3 normal, vec2 uv
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 6));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    out.indexCount = static_cast<uint32_t>(data.indexData.size());
    out.materialName = data.material;
    return true;
}
