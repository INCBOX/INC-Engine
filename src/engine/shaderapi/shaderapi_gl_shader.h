#pragma once
class ShaderProgram {
public:
    unsigned int ID = 0;

    bool Compile(const char* vertexSrc, const char* fragmentSrc);
    void Use() const;
    void Delete();
	bool CompileFromFile(const char* vertexPath, const char* fragmentPath);

private:
    bool CheckCompileErrors(unsigned int shader, const char* type);
    bool CheckLinkErrors(unsigned int program);
};