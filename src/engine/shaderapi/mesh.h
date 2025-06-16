#pragma once
#include <vector>
#include <memory>
#include <glad/glad.h>
#include "shaderapi_gl_vao.h"
#include "shaderapi_gl_buffer.h"
#include "mathlib/matrix.h"

// Simple Mesh class
class Mesh {
public:
    Mesh();
    ~Mesh();

    // Upload geometry data to GPU (positions, normals, texcoords, indices)
    // For simplicity, let's just do positions and indices here
    void Upload(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);

    void Bind() const;
    void Unbind() const;

    // Get count of indices to draw
    size_t GetIndexCount() const;

private:
    std::unique_ptr<VertexArray> m_VAO;
    std::unique_ptr<VertexBuffer> m_VBO;
    std::unique_ptr<VertexBuffer> m_EBO; // Element buffer object for indices

    size_t m_IndexCount = 0;
};
