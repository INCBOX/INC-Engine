#pragma once
class ShaderProgram {
public:
    unsigned int ID = 0;

    bool Compile(const char* vertexSrc, const char* fragmentSrc);
    void Use() const;
    void Delete();

private:
    bool CheckCompileErrors(unsigned int shader, const char* type);
    bool CheckLinkErrors(unsigned int program);
};

// Shader sources declared here so shaderapi_gl.cpp can use them
static const char* vertexShaderSrc = R"glsl(
    #version 330 core
    layout(location = 0) in vec3 aPos;
    layout(location = 1) in vec3 aColor;

    uniform mat4 u_MVP;

    out vec3 vColor;

    void main() {
        gl_Position = u_MVP * vec4(aPos, 1.0);
        vColor = aColor;
    }
)glsl";

static const char* fragmentShaderSrc = R"glsl(
    #version 330 core
    in vec3 vColor;
    out vec4 FragColor;

    void main() {
        FragColor = vec4(vColor, 1.0);
    }
)glsl";
