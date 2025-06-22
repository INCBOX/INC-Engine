#pragma once
#include <cstddef>  // For size_t
#include <glad/glad.h>

class VertexBuffer {
public:
    unsigned int ID = 0;
    unsigned int Target = GL_ARRAY_BUFFER; // Buffer target, default to vertex buffer

    VertexBuffer(unsigned int target = GL_ARRAY_BUFFER);
    ~VertexBuffer();

    void Bind() const;
    void Unbind() const;
    void SetData(const void* data, size_t size) const;
};
