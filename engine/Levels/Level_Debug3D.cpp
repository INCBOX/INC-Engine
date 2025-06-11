// Level_Debug3D.cpp
#include "Level_Debug3D.h"
#include "engine_globals.h"
#include <glad/glad.h>
#include <SDL.h>

#include "mathlib/Mat4.h"
using namespace mathlib;

static GLuint vao = 0;
static GLuint vbo = 0;
static int lineCount = 0;

void Level_Debug3D::Init() {
    float gridSpacing = 1.0f;
    int gridLines = 20; // 10 in each direction

    std::vector<float> vertices;

    // Ground grid on XZ plane
    for (int i = -gridLines / 2; i <= gridLines / 2; ++i) {
        // Lines parallel to X axis
        vertices.push_back(-gridLines / 2 * gridSpacing);
        vertices.push_back(0.0f);
        vertices.push_back(i * gridSpacing);
        vertices.push_back(gridLines / 2 * gridSpacing);
        vertices.push_back(0.0f);
        vertices.push_back(i * gridSpacing);

        // Lines parallel to Z axis
        vertices.push_back(i * gridSpacing);
        vertices.push_back(0.0f);
        vertices.push_back(-gridLines / 2 * gridSpacing);
        vertices.push_back(i * gridSpacing);
        vertices.push_back(0.0f);
        vertices.push_back(gridLines / 2 * gridSpacing);
    }

    // Cube at origin (wireframe lines)
    const float hs = 0.5f;
    const float cube[] = {
        -hs,-hs,-hs,  hs,-hs,-hs,
         hs,-hs,-hs,  hs, hs,-hs,
         hs, hs,-hs, -hs, hs,-hs,
        -hs, hs,-hs, -hs,-hs,-hs,

        -hs,-hs, hs,  hs,-hs, hs,
         hs,-hs, hs,  hs, hs, hs,
         hs, hs, hs, -hs, hs, hs,
        -hs, hs, hs, -hs,-hs, hs,

        -hs,-hs,-hs, -hs,-hs, hs,
         hs,-hs,-hs,  hs,-hs, hs,
         hs, hs,-hs,  hs, hs, hs,
        -hs, hs,-hs, -hs, hs, hs
    };
    vertices.insert(vertices.end(), cube, cube + sizeof(cube) / sizeof(float));
    lineCount = vertices.size() / 3;

    // Upload to GPU
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

void Level_Debug3D::Update(float dt) {
    // No-op for debug
}

void Level_Debug3D::Render() {
    gBasicShader.Bind();
    Mat4 mvp = gProjMatrix * gViewMatrix;
    gBasicShader.SetUniformMat4("u_MVP", mvp.Data());

    glBindVertexArray(vao);
    glDrawArrays(GL_LINES, 0, lineCount);
    glBindVertexArray(0);

    gBasicShader.Unbind();
}

void Level_Debug3D::Unload() {
    if (vao) glDeleteVertexArrays(1, &vao);
    if (vbo) glDeleteBuffers(1, &vbo);
    vao = 0;
    vbo = 0;
}
