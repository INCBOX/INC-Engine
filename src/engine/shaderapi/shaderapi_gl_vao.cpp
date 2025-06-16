// VertexArray

#include "shaderapi/shaderapi_gl_vao.h"
#include <glad/glad.h>

VertexArray::VertexArray() {
    glGenVertexArrays(1, &ID);
}

VertexArray::~VertexArray() {
    if (ID) glDeleteVertexArrays(1, &ID);
}

void VertexArray::Bind() const {
    glBindVertexArray(ID);
}

void VertexArray::Unbind() const {
    glBindVertexArray(0);
}

void VertexArray::AddVertexAttribute(unsigned int index, int size, unsigned int type, bool normalized, size_t stride, const void* pointer) const {
    glVertexAttribPointer(index, size, type, normalized ? GL_TRUE : GL_FALSE, stride, pointer);
    glEnableVertexAttribArray(index);
}
