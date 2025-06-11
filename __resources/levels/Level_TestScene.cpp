#include "runtime_gamedata_path.h"
#include "engine_globals.h"
#include "mathlib/Mat4.h"
#include "mathlib/Vec3.h"
#include <glad/glad.h>
#include <iostream>

#include "Level_TestScene.h"

using namespace mathlib;

void Level_TestScene::Init() {
    float groundVertices[] = {
        -10.0f, 0.0f, -10.0f,
         10.0f, 0.0f, -10.0f,
         10.0f, 0.0f,  10.0f,
        -10.0f, 0.0f, -10.0f,
         10.0f, 0.0f,  10.0f,
        -10.0f, 0.0f,  10.0f
    };

    glGenVertexArrays(1, &vaoGround);
    glGenBuffers(1, &vboGround);

    glBindVertexArray(vaoGround);
    glBindBuffer(GL_ARRAY_BUFFER, vboGround);
    glBufferData(GL_ARRAY_BUFFER, sizeof(groundVertices), groundVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    const char* vertexSrc = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "uniform mat4 uMVP;\n"
        "void main() { gl_Position = uMVP * vec4(aPos, 1.0); }";

    const char* fragmentSrc = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main() { FragColor = vec4(0.3, 0.8, 0.3, 1.0); }";

    unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertexSrc, nullptr);
    glCompileShader(vs);

    unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragmentSrc, nullptr);
    glCompileShader(fs);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vs);
    glAttachShader(shaderProgram, fs);
    glLinkProgram(shaderProgram);

    glDeleteShader(vs);
    glDeleteShader(fs);
}

void Level_TestScene::Render() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgram);

    GLint loc = glGetUniformLocation(shaderProgram, "uMVP");
    Mat4 modelGround = Mat4::Identity();
    Mat4 mvpGround = gProjMatrix * gViewMatrix * modelGround;
    glUniformMatrix4fv(loc, 1, GL_FALSE, mvpGround.Data());

    glBindVertexArray(vaoGround);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
    glUseProgram(0);
}

void Level_TestScene::Update(float deltaTime) {
    // You can keep this empty for now if not used
}

void Level_TestScene::Unload() {
    glDeleteVertexArrays(1, &vaoGround);
    glDeleteBuffers(1, &vboGround);
    glDeleteProgram(shaderProgram);
}