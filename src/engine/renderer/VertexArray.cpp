// VertexArray.cpp
#include "renderer/VertexArray.h"

VertexArray::VertexArray() {
    glGenVertexArrays(1, &m_RendererID);
}

VertexArray::~VertexArray() {
    if (m_RendererID)
        glDeleteVertexArrays(1, &m_RendererID);
}

void VertexArray::Bind() const {
    glBindVertexArray(m_RendererID);
}

void VertexArray::Unbind() const {
    glBindVertexArray(0);
}

void VertexArray::AddVertexAttribute(
    unsigned int index, int size, GLenum type,
    GLboolean normalized, GLsizei stride, const void* pointer)
{
    Bind();
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, size, type, normalized, stride, pointer);
    Unbind();
}