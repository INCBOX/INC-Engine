#include "hud.h"
#include <SDL2/SDL.h>
#include <fstream>
#include <sstream>

HUD::HUD() : quadVAO(0), quadVBO(0), shaderProgram(0) {}

HUD::~HUD() {
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
    glDeleteProgram(shaderProgram);
}

GLuint HUD::CompileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    return shader;
}

GLuint HUD::LoadShader(const char* vertexPath, const char* fragmentPath) {
    std::ifstream vertFile(vertexPath), fragFile(fragmentPath);
    std::stringstream vStream, fStream;
    vStream << vertFile.rdbuf();
    fStream << fragFile.rdbuf();
    std::string vStr = vStream.str(), fStr = fStream.str();
    const char* vSrc = vStr.c_str();
    const char* fSrc = fStr.c_str();

    GLuint vs = CompileShader(GL_VERTEX_SHADER, vSrc);
    GLuint fs = CompileShader(GL_FRAGMENT_SHADER, fSrc);
    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glDeleteShader(vs);
    glDeleteShader(fs);
    return program;
}

void HUD::Init() {
    float quadVertices[] = {
        // pos      // tex
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
    };

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    shaderProgram = LoadShader("hud.vert", "hud.frag");
}

void HUD::RenderQuad(float x, float y, float width, float height, GLuint textureID) {
    glUseProgram(shaderProgram);

    SDL_Window* win = SDL_GL_GetCurrentWindow();
    int w, h;
    SDL_GetWindowSize(win, &w, &h);

    float ortho[16] = {
        2.0f / w, 0, 0, 0,
        0, -2.0f / h, 0, 0,
        0, 0, -1, 0,
        -1, 1, 0, 1
    };
    float model[16] = {
        width, 0, 0, 0,
        0, height, 0, 0,
        0, 0, 1, 0,
        x, y, 0, 1
    };

    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, ortho);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(glGetUniformLocation(shaderProgram, "hudTexture"), 0);

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}