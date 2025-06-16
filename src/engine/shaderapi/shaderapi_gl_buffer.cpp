#include "shaderapi/shaderapi_gl_buffer.h"
#include <glad/glad.h>

VertexBuffer::VertexBuffer() {
    glGenBuffers(1, &ID);
}

VertexBuffer::~VertexBuffer() {
    if (ID) glDeleteBuffers(1, &ID);
}

void VertexBuffer::Bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, ID);
}

void VertexBuffer::Unbind() const {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::SetData(const void* data, size_t size) const {
    glBindBuffer(GL_ARRAY_BUFFER, ID);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}
