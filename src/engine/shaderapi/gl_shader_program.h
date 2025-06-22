#pragma once

class ShaderProgram {
public:
    unsigned int ID = 0;

    bool Compile(const char* vertexSrc, const char* fragmentSrc);
    bool CompileFromFile(const char* vertexPath, const char* fragmentPath);
	
    void Use() const;
    void Delete();
	
    int GetMVPLocation() const { return m_MVPLocation; }
	
private:
    int m_MVPLocation = -1;

    bool CheckCompileErrors(unsigned int shader, const char* type);
    bool CheckLinkErrors(unsigned int program);
};
