
#include "engine_Render.h"
#include "engine_Globals.h"
#include "runtime_gamedata_path.h"
#include "engine_Shaders.h"
#include "engine_Render_Debug.h"
#include <glad/glad.h>
#include <string>
#include <iostream>

using namespace mathlib;

static GLuint vao = 0, vbo = 0, ebo = 0;
static Shader shader;

static float vertices[] = {
    // Floor (2 triangles)
    -10, 0, -10,   0.4f, 0.4f, 0.4f,
     10, 0, -10,   0.4f, 0.4f, 0.4f,
     10, 0,  10,   0.4f, 0.4f, 0.4f,
    -10, 0,  10,   0.4f, 0.4f, 0.4f,

    // Cube side (1 quad)
    -1, 0, -1,     1, 0, 0,
    -1, 1, -1,     1, 0, 0,
    -1, 1,  0,     1, 0, 0,
    -1, 0,  0,     1, 0, 0,
};

static unsigned int indices[] = {
    0, 1, 2, 2, 3, 0,  // floor
    4, 5, 6, 6, 7, 4   // cube face
};

void Render_CreateCubeFloor() {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    shader.Load(gamedata::Shader("basic.vert"), gamedata::Shader("basic.frag"));
}

void Render_DrawScene(const Mat4& mvp) {
	std::cout << "[Render] DrawScene CALLED\n";
    shader.Bind();
    shader.SetUniformMat4("uMVP", mvp.Data());

    DebugDrawCall(mvp, shader.GetUniformLocation("uMVP"), shader.GetProgram(), 12);

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    shader.Unbind();
}

void Render_Cleanup() {
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteVertexArrays(1, &vao);
    // Shader cleanup is automatic in Shader::~Shader()
}
