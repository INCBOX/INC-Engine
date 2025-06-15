// VertexBuffer.cpp
#include "VertexBuffer.h"

VertexBuffer::~VertexBuffer() {
    if (m_RendererID) glDeleteBuffers(1, &m_RendererID);
}

void VertexBuffer::Bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
}

void VertexBuffer::Unbind() const {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::SetData(const void* data, unsigned int size, GLenum usage) {
    if (!m_RendererID)
        glGenBuffers(1, &m_RendererID);

    Bind();
    glBufferData(GL_ARRAY_BUFFER, size, data, usage);
}