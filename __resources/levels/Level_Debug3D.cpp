#include "Levels/Level_Debug3D.h"  // ✅ REQUIRED for class definition
#include "engine_globals.h"
#include <glad/glad.h>
#include <iostream>

static GLuint vao = 0, vbo = 0;

void Level_Debug3D::Init() {
    std::cout << "[Level_Debug3D] Init()" << std::endl;

    float vertices[] = {
        // positions         // colors
        -0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f,  // red
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,  // green
         0.0f,  0.5f, 0.0f,   0.0f, 0.0f, 1.0f   // blue
    };

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    std::cout << "[Level_Debug3D] VAO/VBO setup complete\n";
}

void Level_Debug3D::Update(float deltaTime) {
    // Optional game logic here
}

void Level_Debug3D::Render() {
    gBasicShader.Bind();
    gBasicShader.SetUniformMat4("uModel", mathlib::Mat4::Identity().Data());
    gBasicShader.SetUniformMat4("uView", gViewMatrix.Data());
    gBasicShader.SetUniformMat4("uProjection", gProjMatrix.Data());

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);

    gBasicShader.Unbind();

    std::cout << "[Debug3D] Rendering colorful triangle" << std::endl;
}

void Level_Debug3D::Shutdown() {
    if (vao) glDeleteVertexArrays(1, &vao);
    if (vbo) glDeleteBuffers(1, &vbo);
    vao = vbo = 0;

    std::cout << "[Level_Debug3D] Shutdown complete\n";
}
