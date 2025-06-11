#pragma once
#include <string>
#include <unordered_map>
#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <sstream>

class Shader {
public:
    Shader();
    ~Shader();

    bool Load(const std::string& vertexPath, const std::string& fragmentPath);

    void Bind() const;
    void Unbind() const;

    GLuint GetProgram() const;

    void SetUniformMat4(const std::string& name, const float* matrix);
    void SetUniformVec3(const std::string& name, float x, float y, float z);
    void SetUniformInt(const std::string& name, int value);
    void SetUniformFloat(const std::string& name, float value);

private:
    GLuint programID;
    std::unordered_map<std::string, GLint> uniformLocationCache;

    bool ReadFile(const std::string& path, std::string& outCode);
    GLuint CompileShader(GLenum type, const char* source);
    bool CreateProgram(const char* vertexSrc, const char* fragmentSrc);
    GLint GetUniformLocation(const std::string& name);
};
