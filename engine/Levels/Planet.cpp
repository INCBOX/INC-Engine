
#include "Planet.h"
#include <glad/glad.h>

static GLuint vao = 0, vbo = 0;
static float planetSize = 10.0f;
static Vec3 planetPos = { 0.0f, 0.0f, -50.0f };

void InitPlanet() {
    float size = planetSize;
    float verts[] = {
        -size, -size, -size,  size, -size, -size,  size,  size, -size,
         size,  size, -size, -size,  size, -size, -size, -size, -size,

        -size, -size,  size,  size, -size,  size,  size,  size,  size,
         size,  size,  size, -size,  size,  size, -size, -size,  size,

        -size,  size,  size, -size,  size, -size, -size, -size, -size,
        -size, -size, -size, -size, -size,  size, -size,  size,  size,

         size,  size,  size,  size,  size, -size,  size, -size, -size,
         size, -size, -size,  size, -size,  size,  size,  size,  size,

        -size, -size, -size,  size, -size, -size,  size, -size,  size,
         size, -size,  size, -size, -size,  size, -size, -size, -size,

        -size,  size, -size,  size,  size, -size,  size,  size,  size,
         size,  size,  size, -size,  size,  size, -size,  size, -size,
    };

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}

void RenderPlanet(const Mat4& view, const Mat4& projection) {
    Mat4 model = Mat4::translate(planetPos);
    Mat4 mvp = projection * view * model;

    GLint shader = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &shader);
    if (shader == 0) return;

    GLint loc = glGetUniformLocation(shader, "u_mvp");
    glUniformMatrix4fv(loc, 1, GL_FALSE, mvp.m);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}
