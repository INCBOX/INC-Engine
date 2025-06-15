// VertexArray.h
#pragma once
#include <glad/glad.h>
#include "VertexBuffer.h"

class VertexArray {
private:
    unsigned int m_RendererID = 0;
public:
    VertexArray();
    ~VertexArray();

    void Bind() const;
    void Unbind() const;

    // Setup vertex attributes for the currently bound VBO
    void AddVertexAttribute(
        unsigned int index,       // layout location in shader
        int size,                 // number of components (e.g. 3 for vec3)
        GLenum type,              // data type, e.g. GL_FLOAT
        GLboolean normalized,
        GLsizei stride,
        const void* pointer
    );
};
