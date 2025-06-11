#include "Level_Debug3D.h"
#include "engine_globals.h"
#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <iostream>
#include "mathlib/Mat4.h"

using namespace mathlib;

void Level_Debug3D::Init() {
    std::cout << "[Level_Debug3D] Init()\n";
}

void Level_Debug3D::Update(float dt) {
    // No logic needed for now
}

void Level_Debug3D::Render() {
    std::cout << "[Debug3D] Rendering large triangle\n";

    Mat4 model = Mat4::Identity();
    gBasicShader.SetUniformMat4("uModel", model.Data());

    float verts[] = {
        -10.0f, -10.0f, -20.0f,
         10.0f, -10.0f, -20.0f,
         0.0f,   10.0f, -20.0f
    };

    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}


void Level_Debug3D::Unload() {
    // No resources to release for now
}