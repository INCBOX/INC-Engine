// MarsLevel_1.cpp
#include "MarsLevel_1.h"
#include <glad/glad.h>
#include <stb_image.h>
#include <iostream>
#include <vector>
#include <cmath>

#include "../Math/Math.h"

static GLuint vao = 0, vbo = 0, ebo = 0, shaderProgram = 0, textureID = 0;

const char* marsVertexShader = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}
)";

const char* marsFragmentShader = R"(
#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D marsTexture;

void main() {
    vec4 tex = texture(marsTexture, TexCoord);
    float fogFactor = clamp(gl_FragCoord.z / gl_FragCoord.w / 100.0, 0.0, 1.0);
    vec3 fogColor = vec3(0.5, 0.2, 0.1);
    FragColor = mix(tex, vec4(fogColor, 1.0), fogFactor);
}
)";

static GLuint Compile(GLenum type, const char* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        std::cerr << "Shader Error: " << log << std::endl;
    }
    return shader;
}

void LoadMarsLevel_1() {
    GLuint vs = Compile(GL_VERTEX_SHADER, marsVertexShader);
    GLuint fs = Compile(GL_FRAGMENT_SHADER, marsFragmentShader);
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vs);
    glAttachShader(shaderProgram, fs);
    glLinkProgram(shaderProgram);
    glDeleteShader(vs);
    glDeleteShader(fs);

    const int gridSize = 100;
    const float spacing = 1.0f;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    for (int z = 0; z <= gridSize; ++z) {
        for (int x = 0; x <= gridSize; ++x) {
            float fx = x * spacing;
            float fz = z * spacing;
            float height = 0.3f * sinf(x * 0.3f) * cosf(z * 0.4f);
            vertices.push_back(fx);
            vertices.push_back(height);
            vertices.push_back(fz);
            vertices.push_back(x / (float)gridSize);
            vertices.push_back(z / (float)gridSize);
        }
    }

    for (int z = 0; z < gridSize; ++z) {
        for (int x = 0; x < gridSize; ++x) {
            int i0 = z * (gridSize + 1) + x;
            int i1 = i0 + 1;
            int i2 = i0 + (gridSize + 1);
            int i3 = i2 + 1;
            indices.push_back(i0);
            indices.push_back(i2);
            indices.push_back(i1);
            indices.push_back(i1);
            indices.push_back(i2);
            indices.push_back(i3);
        }
    }

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Load Mars texture
    int w, h, ch;
    unsigned char* img = stbi_load("bin/resources/textures/mars/mars_ground_diffuse.png", &w, &h, &ch, 0);
    if (!img) {
        std::cerr << "Failed to load Mars texture!" << std::endl;
    } else {
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(img);
    }
}

void RenderMarsLevel_1(const Mat4& view, const Mat4& projection) {
    glUseProgram(shaderProgram);
    glBindVertexArray(vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(glGetUniformLocation(shaderProgram, "marsTexture"), 0);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, view.m);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, projection.m);

    glDrawElements(GL_TRIANGLES, 6 * 100 * 100, GL_UNSIGNED_INT, 0);
}

void UnloadMarsLevel_1() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteProgram(shaderProgram);
    glDeleteTextures(1, &textureID);
}
