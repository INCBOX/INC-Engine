#pragma once

#include <vector>
#include <memory>
#include <glad/glad.h>

#include "shaderapi/geometry_shaderapi.h"
#include "shaderapi/shaderapi_gl_vao.h"
#include "shaderapi/shaderapi_gl_buffer.h"

class GeometryGL : public IGeometry {
public:
    GeometryGL();
    ~GeometryGL();

    // Upload geometry data to GPU (positions and indices)
    void Upload(const std::vector<float>& vertices, const std::vector<unsigned int>& indices) override;
    void Bind() const override;
    void Unbind() const override;
    size_t GetIndexCount() const override;

private:
    std::unique_ptr<VertexArray> m_VAO;
    std::unique_ptr<VertexBuffer> m_VBO; // Vertex data (positions, etc.)
    std::unique_ptr<VertexBuffer> m_EBO; // Element buffer object for indices

    size_t m_IndexCount = 0;
    bool m_Uploaded = false; // kills performance fix
};