#include "TestLevel2.h"
#include "../Math/Math.h"
#include "../Skybox/Skybox.h"
#include <glad/glad.h>
#include <iostream>

static GLuint vaoFloor = 0, vboFloor = 0;
static GLuint vaoCubes = 0, vboCubes = 0;
static GLuint shaderProgram = 0;

const char* vsSource = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
uniform mat4 view;
uniform mat4 projection;
void main() {
    gl_Position = projection * view * vec4(aPos, 1.0);
}
)";

const char* fsSource = R"(
#version 330 core
out vec4 FragColor;
void main() {
    FragColor = vec4(0.6, 0.6, 0.8, 1.0);
}
)";

static const float floorVertices[] = {
    -5.0f, 0.0f, -5.0f,
    -5.0f, 0.0f,  5.0f,
     5.0f, 0.0f,  5.0f,
     5.0f, 0.0f,  5.0f,
     5.0f, 0.0f, -5.0f,
    -5.0f, 0.0f, -5.0f
};

static const float cubeVertices[] = {
    -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f
};

static GLuint compileShader(GLenum type, const char* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    return shader;
}

void LoadTestLevel_2() {
    std::cout << "Test level loaded\n";
    InitSkybox();

    GLuint vs = compileShader(GL_VERTEX_SHADER, vsSource);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fsSource);
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vs);
    glAttachShader(shaderProgram, fs);
    glLinkProgram(shaderProgram);
    glDeleteShader(vs);
    glDeleteShader(fs);

    glGenVertexArrays(1, &vaoFloor);
    glGenBuffers(1, &vboFloor);
    glBindVertexArray(vaoFloor);
    glBindBuffer(GL_ARRAY_BUFFER, vboFloor);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    glGenVertexArrays(1, &vaoCubes);
    glGenBuffers(1, &vboCubes);
    glBindVertexArray(vaoCubes);
    glBindBuffer(GL_ARRAY_BUFFER, vboCubes);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);
}

void RenderLevel(const Mat4& view, const Mat4& projection) {
    // glDepthFunc(GL_LEQUAL);
    // RenderSkybox(view, projection);
    // glDepthFunc(GL_LESS);

    glUseProgram(shaderProgram);
    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, view.m);
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, projection.m);

    glBindVertexArray(vaoFloor);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(vaoCubes);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void UnLoadTestLevel_2() {
    CleanupSkybox();
    glDeleteProgram(shaderProgram);
    glDeleteVertexArrays(1, &vaoFloor);
    glDeleteBuffers(1, &vboFloor);
    glDeleteVertexArrays(1, &vaoCubes);
    glDeleteBuffers(1, &vboCubes);
}
