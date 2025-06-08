#include "TestLevel_1.h"
#include "../Math/Math.h"
#include <glad/glad.h>
#include <vector>
#include <iostream>

// Hardcoded basic shaders
const char* vertexShaderSrc = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
uniform mat4 view;
uniform mat4 projection;
void main() {
    gl_Position = projection * view * vec4(aPos, 1.0);
}
)";

const char* fragmentShaderSrc = R"(
#version 330 core
out vec4 FragColor;
void main() {
    FragColor = vec4(1.0, 0.5, 0.2, 1.0); // Orange
}
)";

// Cube data
static GLuint vao = 0, vbo = 0;
static GLuint shaderProgram = 0;

static float cubeVertices[] = {
    -1, -1, -1,   1, -1, -1,   1,  1, -1,  -1,  1, -1, // back
    -1, -1,  1,   1, -1,  1,   1,  1,  1,  -1,  1,  1  // front
};

static unsigned int cubeIndices[] = {
    0, 1, 2, 2, 3, 0,       // back face
    4, 5, 6, 6, 7, 4,       // front face
    0, 4, 7, 7, 3, 0,       // left face
    1, 5, 6, 6, 2, 1,       // right face
    3, 2, 6, 6, 7, 3,       // top face
    0, 1, 5, 5, 4, 0        // bottom face
};

static GLuint ebo = 0;

static GLuint CompileShader(GLenum type, const char* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        std::cerr << "[Shader Error] " << log << std::endl;
    }

    return shader;
}

void LoadTestLevel_1()
{
    // Compile and link shaders
    GLuint vs = CompileShader(GL_VERTEX_SHADER, vertexShaderSrc);
    GLuint fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSrc);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vs);
    glAttachShader(shaderProgram, fs);
    glLinkProgram(shaderProgram);

    // Clean up shaders (linked already)
    glDeleteShader(vs);
    glDeleteShader(fs);

    // Create VAO, VBO, EBO
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void RenderTestLevel_1(const Mat4& view, const Mat4& projection)
{
    glUseProgram(shaderProgram);

    // Upload matrices
    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, view.toGLMatrix());
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, projection.toGLMatrix());

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void UnloadTestLevel_1()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteProgram(shaderProgram);
}