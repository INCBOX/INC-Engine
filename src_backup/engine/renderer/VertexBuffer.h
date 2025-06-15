// VertexBuffer.h
#pragma once
#include <glad/glad.h>

class VertexBuffer {
private:
    unsigned int m_RendererID = 0;
public:
    VertexBuffer() = default;
    ~VertexBuffer();

    void Bind() const;
    void Unbind() const;

    // Upload data to GPU (dynamic/static draw)
    void SetData(const void* data, unsigned int size, GLenum usage = GL_STATIC_DRAW);
};