#include "shaderapi/shaderapi_gl_shader.h"
#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <sstream>

bool ShaderProgram::CompileFromFile(const char* vertexPath, const char* fragmentPath) {
    std::ifstream vFile(vertexPath);
    std::ifstream fFile(fragmentPath);

    if (!vFile || !fFile) {
        std::cerr << "[Shader] Failed to open shader files\n";
        return false;
    }

    std::stringstream vStream, fStream;
    vStream << vFile.rdbuf();
    fStream << fFile.rdbuf();

    std::string vSource = vStream.str();
    std::string fSource = fStream.str();

    bool success = Compile(vSource.c_str(), fSource.c_str());

    m_MVPLocation = glGetUniformLocation(ID, "u_MVP");
    if (m_MVPLocation == -1) {
        std::cerr << "Warning: u_MVP uniform not found\n";
    }

    return success;
}



bool ShaderProgram::Compile(const char* vertexSrc, const char* fragmentSrc) {
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSrc, nullptr);
    glCompileShader(vertexShader);
    if (!CheckCompileErrors(vertexShader, "VERTEX")) return false;

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSrc, nullptr);
    glCompileShader(fragmentShader);
    if (!CheckCompileErrors(fragmentShader, "FRAGMENT")) {
        glDeleteShader(vertexShader);
        return false;
    }

    ID = glCreateProgram();
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    glLinkProgram(ID);
    if (!CheckLinkErrors(ID)) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return false;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return true;
}

void ShaderProgram::Use() const {
    glUseProgram(ID);
}

void ShaderProgram::Delete() {
    if (ID) {
        glDeleteProgram(ID);
        ID = 0;
    }
}

bool ShaderProgram::CheckCompileErrors(unsigned int shader, const char* type) {
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "[GL] Shader compile error (" << type << "): " << infoLog << "\n";
        return false;
    }
    return true;
}

bool ShaderProgram::CheckLinkErrors(unsigned int program) {
    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "[GL] Program link error: " << infoLog << "\n";
        return false;
    }
    return true;
}
