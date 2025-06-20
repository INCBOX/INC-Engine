#pragma once

#include <cstddef>  // For size_t

// Represents a Vertex Array Object (VAO) in OpenGL.
// Encapsulates vertex attribute setup and binding behavior.
class VertexArray {
public:
    unsigned int ID = 0;
	
    VertexArray();
    ~VertexArray();

    // Binds the VAO for use
    void Bind() const;

    // Unbinds the currently bound VAO
    void Unbind() const;

     void AddVertexAttribute(unsigned int index, int size, unsigned int type, bool normalized, size_t stride, const void* pointer) const;
};