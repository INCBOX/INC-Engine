#include <GL/glew.h>
#include "testrender.h"
#include "mathlib/matrix.h"
#include "shaderlib/shader.h" // Assume you have a shader loader

static GLuint cubeVAO = 0;
static GLuint cubeVBO = 0;
static GLuint cubeEBO = 0;
static GLuint cubeShader = 0;

static float cubeVerts[] = {
    -0.5f, -0.5f, -0.5f,  // 0
     0.5f, -0.5f, -0.5f,  // 1
     0.5f,  0.5f, -0.5f,  // 2
    -0.5f,  0.5f, -0.5f,  // 3
    -0.5f, -0.5f,  0.5f,  // 4
     0.5f, -0.5f,  0.5f,  // 5
     0.5f,  0.5f,  0.5f,  // 6
    -0.5f,  0.5f,  0.5f   // 7
};

static uint32_t cubeIndices[] = {
    0, 1, 2, 2, 3, 0, // back
    4, 5, 6, 6, 7, 4, // front
    0, 4, 7, 7, 3, 0, // left
    1, 5, 6, 6, 2, 1, // right
    3, 2, 6, 6, 7, 3, // top
    0, 1, 5, 5, 4, 0  // bottom
};

void R_InitTestCube()
{
    cubeShader = LoadShader("shaders/testcube.vert", "shaders/testcube.frag");

    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glGenBuffers(1, &cubeEBO);

    glBindVertexArray(cubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerts), cubeVerts, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindVertexArray(0);
}

void R_DrawTestCube(const Mat4& view, const Mat4& proj)
{
    glUseProgram(cubeShader);

    Mat4 model = Mat4::Identity();

    glUniformMatrix4fv(glGetUniformLocation(cubeShader, "u_model"), 1, GL_FALSE, model.Data());
    glUniformMatrix4fv(glGetUniformLocation(cubeShader, "u_view"), 1, GL_FALSE, view.Data());
    glUniformMatrix4fv(glGetUniformLocation(cubeShader, "u_proj"), 1, GL_FALSE, proj.Data());

    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
