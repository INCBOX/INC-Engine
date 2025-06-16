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

// Shader sources declared here so shaderapi_gl.cpp can use them
// vertex shader: only position, output fixed color
static const char* vertexShaderSrc = R"glsl(
    #version 330 core
    layout(location = 0) in vec3 aPos;

    uniform mat4 u_MVP;

    out vec3 vColor;

    void main() {
        gl_Position = u_MVP * vec4(aPos, 1.0);
        vColor = vec3(1.0, 0.5, 0.0); // orange color hardcoded
    }
)glsl";

// fragment shader: use passed color
static const char* fragmentShaderSrc = R"glsl(
    #version 330 core
    in vec3 vColor;
    out vec4 FragColor;

    void main() {
        FragColor = vec4(vColor, 1.0);
    }
)glsl";
