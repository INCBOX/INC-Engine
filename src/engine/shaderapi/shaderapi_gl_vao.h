#pragma once
#include <cstddef>  // For size_t
class VertexArray {
public:
    unsigned int ID = 0;

    VertexArray();
    ~VertexArray();

    void Bind() const;
    void Unbind() const;
    void AddVertexAttribute(unsigned int index, int size, unsigned int type, bool normalized, size_t stride, const void* pointer) const;
};
