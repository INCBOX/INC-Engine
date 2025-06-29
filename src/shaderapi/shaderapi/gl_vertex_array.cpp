// VertexArray

#include "shaderapi/gl_vertex_array.h"
#include <glad/glad.h>

// Constructor: generate VAO
VertexArray::VertexArray() {
    glGenVertexArrays(1, &ID);
}

// Destructor: delete VAO
VertexArray::~VertexArray() {
    if (ID) glDeleteVertexArrays(1, &ID);
}

// Bind VAO
void VertexArray::Bind() const {
    glBindVertexArray(ID);
}

// Unbind VAO
void VertexArray::Unbind() const {
    glBindVertexArray(0);
}

void VertexArray::AddVertexAttribute(unsigned int index, int size, unsigned int type, bool normalized, size_t stride, const void* pointer) const {
    glVertexAttribPointer(index, size, type, normalized ? GL_TRUE : GL_FALSE, stride, pointer);
    glEnableVertexAttribArray(index);
}