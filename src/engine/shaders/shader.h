#pragma once
#include <string>

class ShaderProgram {
public:
    ShaderProgram() = default;
    ~ShaderProgram();

    bool Compile(const char* vertexSrc, const char* fragmentSrc);
    void Use() const;
    void Delete();

private:
    unsigned int ID = 0;

    bool CheckCompileErrors(unsigned int shader, const char* type);
    bool CheckLinkErrors(unsigned int program);
};
