#pragma once

#include <vector>
#include <memory>
#include <glad/glad.h>

#include "shaderapi/igpu_mesh.h"
#include "shaderapi/gl_vertex_array.h"
#include "shaderapi/gl_buffer.h"

class GLMesh : public IGPUMesh {
public:
    GLMesh();
    ~GLMesh() override;

    void Upload(const std::vector<float>& vertices, const std::vector<unsigned int>& indices) override;
    void Bind() const override;
    void Unbind() const override;
    size_t GetIndexCount() const override;

    GLMesh(GLMesh&&) = default;
    GLMesh& operator=(GLMesh&&) = default;

private:
    std::unique_ptr<VertexArray> m_VAO;
    std::unique_ptr<VertexBuffer> m_VBO;
    std::unique_ptr<VertexBuffer> m_EBO;

    size_t m_IndexCount = 0;
    bool m_Uploaded = false; // Flag to avoid redundant uploads
};