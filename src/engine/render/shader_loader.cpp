// shader_loader.cpp - Source-style shader loader using FS_ResolvePath
#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include "filesystem_stdio.h" // for FS_ResolvePath

unsigned int LoadShaderFromFile(const std::string& vertexRelPath, const std::string& fragmentRelPath) {
    std::string vertexFull = FS_ResolvePath(vertexRelPath);
    std::string fragmentFull = FS_ResolvePath(fragmentRelPath);

    std::ifstream vsFile(vertexFull);
    std::ifstream fsFile(fragmentFull);

    if (!vsFile.is_open() || !fsFile.is_open()) {
        std::cerr << "[ShaderLoader] Failed to open shader files: "
                  << vertexRelPath << ", " << fragmentRelPath << "\n";
        return 0;
    }

    std::stringstream vsStream, fsStream;
    vsStream << vsFile.rdbuf();
    fsStream << fsFile.rdbuf();

    std::string vsCode = vsStream.str();
    std::string fsCode = fsStream.str();

    const char* vShaderCode = vsCode.c_str();
    const char* fShaderCode = fsCode.c_str();

    unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);

    int success;
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cerr << "[ShaderLoader] Vertex shader compilation failed:\n" << infoLog << "\n";
        return 0;
    }

    unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);

    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cerr << "[ShaderLoader] Fragment shader compilation failed:\n" << infoLog << "\n";
        return 0;
    }

    unsigned int program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cerr << "[ShaderLoader] Shader linking failed:\n" << infoLog << "\n";
        return 0;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return program;
}
