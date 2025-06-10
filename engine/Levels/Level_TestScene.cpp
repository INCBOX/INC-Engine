#include "runtime_gamedata_path.h"
#include "math.h"
#include "Level_TestScene.h"
#include <glad/glad.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

// External camera matrices from the engine
extern Mat4 gViewMatrix;
extern Mat4 gProjMatrix;

static GLuint vaoTriangle = 0, vboTriangle = 0;
static GLuint vaoGround = 0, vboGround = 0;
static GLuint shaderProgram = 0;

void Level_TestScene::Init() {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDisable(GL_CULL_FACE);  // Optional for testing

    auto LoadShaderSource = [](const std::string& path) -> std::string {
        std::ifstream file(path);
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    };

    auto CompileShader = [](GLenum type, const std::string& src) -> GLuint {
        GLuint shader = glCreateShader(type);
        const char* cstr = src.c_str();
        glShaderSource(shader, 1, &cstr, nullptr);
        glCompileShader(shader);

        GLint success = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            GLint length = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
            std::string log(length, ' ');
            glGetShaderInfoLog(shader, length, nullptr, &log[0]);
            std::cerr << "[Shader compile error] " << log << std::endl;
        }
        return shader;
    };

    std::string vertexSrc = LoadShaderSource(gamedata::Shader("basic.vert"));
    std::string fragmentSrc = LoadShaderSource(gamedata::Shader("basic.frag"));

    GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSrc);
    GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSrc);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    GLint linked = 0;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linked);
    if (!linked) {
        GLint length = 0;
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &length);
        std::string log(length, ' ');
        glGetProgramInfoLog(shaderProgram, length, nullptr, &log[0]);
        std::cerr << "[Program link error] " << log << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Triangle vertex data
    float triangleVerts[] = {
         0.0f,  1.0f, -0.5f,
        -1.0f, -1.0f, -0.5f,
         1.0f, -1.0f, -0.5f
    };

    // Ground plane vertices (two triangles forming a quad)
    float groundVerts[] = {
        -10.0f, -1.0f, -10.0f,
         10.0f, -1.0f, -10.0f,
         10.0f, -1.0f,  10.0f,

        -10.0f, -1.0f, -10.0f,
         10.0f, -1.0f,  10.0f,
        -10.0f, -1.0f,  10.0f,
    };

    // Triangle setup
    glGenVertexArrays(1, &vaoTriangle);
    glGenBuffers(1, &vboTriangle);
    glBindVertexArray(vaoTriangle);
    glBindBuffer(GL_ARRAY_BUFFER, vboTriangle);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVerts), triangleVerts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

    // Ground setup
    glGenVertexArrays(1, &vaoGround);
    glGenBuffers(1, &vboGround);
    glBindVertexArray(vaoGround);
    glBindBuffer(GL_ARRAY_BUFFER, vboGround);
    glBufferData(GL_ARRAY_BUFFER, sizeof(groundVerts), groundVerts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

    glBindVertexArray(0);
}

void Level_TestScene::Render() {
    std::cout << "[Render] Level_TestScene::Render called\n";

    glUseProgram(shaderProgram);

    GLint loc = glGetUniformLocation(shaderProgram, "uMVP");

    // === Ground ===
    Mat4 modelGround = Mat4::identity();
    Mat4 mvpGround = gProjMatrix * gViewMatrix * modelGround;
    glUniformMatrix4fv(loc, 1, GL_FALSE, mvpGround.toGLMatrix());
    glBindVertexArray(vaoGround);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // === Triangle ===
    Mat4 modelTriangle = Mat4::identity();
    Mat4 mvpTriangle = gProjMatrix * gViewMatrix * modelTriangle;
    glUniformMatrix4fv(loc, 1, GL_FALSE, mvpTriangle.toGLMatrix());
    glBindVertexArray(vaoTriangle);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glBindVertexArray(0);
    glUseProgram(0);
}

void Level_TestScene::Update(float) {
    // Add per-frame logic here if needed
}

void Level_TestScene::Unload() {
    // Optional: glDeleteBuffers, glDeleteVertexArrays if needed
}