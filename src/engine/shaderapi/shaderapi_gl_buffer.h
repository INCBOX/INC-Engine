#pragma once
#include <cstddef>  // For size_t
class VertexBuffer {
public:
    unsigned int ID = 0;

    VertexBuffer();
    ~VertexBuffer();

    void Bind() const;
    void Unbind() const;
    void SetData(const void* data, size_t size) const;
};
