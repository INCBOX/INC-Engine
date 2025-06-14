#pragma once
#include <string>
#include <vector>
#include <glad/glad.h>

class MaterialGL {
public:
    MaterialGL() = default;
    ~MaterialGL();

    // Loads shader from vertex and fragment shader paths
    bool LoadShader(const std::string& vertexPath, const std::string& fragmentPath);

    // Sets the base texture OpenGL handle
    void SetTexture(GLuint texID);

    // Binds shader and texture for rendering
    void Bind() const;

    GLuint GetShaderID() const { return shaderProgram; }

private:
    GLuint shaderProgram = 0;
    GLuint baseTexture = 0;

    GLuint CompileShader(GLenum type, const std::string& source);
    std::string ReadFile(const std::string& path);
};
