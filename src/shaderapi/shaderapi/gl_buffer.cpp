// (VertexBuffer)

#include "shaderapi/gl_buffer.h"

VertexBuffer::VertexBuffer(unsigned int target) : Target(target) {
    glGenBuffers(1, &ID);
}

VertexBuffer::~VertexBuffer() {
    if (ID) glDeleteBuffers(1, &ID);
}

void VertexBuffer::Bind() const {
    glBindBuffer(Target, ID);
}

void VertexBuffer::Unbind() const {
    glBindBuffer(Target, 0);
}

void VertexBuffer::SetData(const void* data, size_t size) const {
    glBindBuffer(Target, ID);
    glBufferData(Target, size, data, GL_STATIC_DRAW);
}
