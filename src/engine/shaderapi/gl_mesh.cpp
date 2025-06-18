#include "shaderapi/gl_mesh.h"  // MeshGL class declaration
#include <glad/glad.h>           // For GL constants
#include <memory>                // For unique_ptr, if needed


MeshGL::MeshGL() {
    m_VAO = std::make_unique<VertexArray>();
    m_VBO = std::make_unique<VertexBuffer>(GL_ARRAY_BUFFER);         // Vertex buffer
    m_EBO = std::make_unique<VertexBuffer>(GL_ELEMENT_ARRAY_BUFFER);  // Index buffer
}

MeshGL::~MeshGL() {
    // Unique pointers auto-cleanup
}

void MeshGL::Upload(const std::vector<float>& vertices, const std::vector<unsigned int>& indices) {
    if (m_Uploaded)
        return; // Already uploaded once, don't do it again

    m_IndexCount = indices.size();

    m_VAO->Bind();

    m_VBO->Bind();
    m_VBO->SetData(vertices.data(), vertices.size() * sizeof(float));

    m_EBO->Bind();
    m_EBO->SetData(indices.data(), indices.size() * sizeof(unsigned int));

    m_VAO->AddVertexAttribute(0, 3, GL_FLOAT, false, 3 * sizeof(float), (void*)0);

    m_VAO->Unbind();
    m_VBO->Unbind();
    m_EBO->Unbind();

    m_Uploaded = true; // uploaded for performance
}


void MeshGL::Bind() const {
    m_VAO->Bind();
}

void MeshGL::Unbind() const {
    m_VAO->Unbind();
}

size_t MeshGL::GetIndexCount() const {
    return m_IndexCount;
}
