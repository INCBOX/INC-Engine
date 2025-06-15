#pragma once
#include <glad/glad.h>

class Mesh {
public:
    Mesh();
    ~Mesh();

    // Setup vertex data: positions + colors for example
    void Setup(float* vertices, size_t size);

    // Draw the mesh (bind VAO, call glDrawArrays)
    void Draw() const;

    void Cleanup();

private:
    unsigned int VAO = 0;
    unsigned int VBO = 0;
    size_t vertexCount = 0;
};
