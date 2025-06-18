#pragma once

#include <vector>
#include <memory>
#include <glad/glad.h>

#include "shaderapi/imesh.h"
#include "shaderapi_gl_vao.h"
#include "shaderapi_gl_buffer.h"
#include "mathlib/matrix.h"

class MeshGL : public IMesh {
public:
    MeshGL();
    ~MeshGL();

    // Upload geometry data to GPU (positions and indices)
    void Upload(const std::vector<float>& vertices, const std::vector<unsigned int>& indices) override;
    void Bind() const override;
    void Unbind() const override;
    size_t GetIndexCount() const override;

private:
    std::unique_ptr<VertexArray> m_VAO;
    std::unique_ptr<VertexBuffer> m_VBO;
    std::unique_ptr<VertexBuffer> m_EBO; // Element buffer object for indices

    size_t m_IndexCount = 0;
};
