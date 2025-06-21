#pragma once

#include <vector>
#include <memory>
#include <glad/glad.h>

#include "shaderapi/static_mesh_shaderapi.h"
#include "shaderapi/shaderapi_gl_vao.h"
#include "shaderapi/shaderapi_gl_buffer.h"

class GeometryGL : public IGeometry {
public:
    GeometryGL();
    ~GeometryGL() override;

    void Upload(const std::vector<float>& vertices, const std::vector<unsigned int>& indices) override;
    void Bind() const override;
    void Unbind() const override;
    size_t GetIndexCount() const override;

    GeometryGL(GeometryGL&&) = default;
    GeometryGL& operator=(GeometryGL&&) = default;

private:
    std::unique_ptr<VertexArray> m_VAO;
    std::unique_ptr<VertexBuffer> m_VBO;
    std::unique_ptr<VertexBuffer> m_EBO;

    size_t m_IndexCount = 0;
    bool m_Uploaded = false; // Flag to avoid redundant uploads
};