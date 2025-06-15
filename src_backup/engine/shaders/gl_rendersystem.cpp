// Updated gl_rendersystem.cpp — cleaned up to act as a backend only
// Triangle rendering will now be handled by renderer.cpp (higher layer)

#include "shaders/gl_rendersystem.h"
#include <glad/glad.h>
#include <iostream>
#include "mathlib/matrix.h"
#include "mathlib/vector.h"

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

bool RenderSystemGL::ShaderProgram::Compile(const char* vertexSrc, const char* fragmentSrc) {
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

void RenderSystemGL::ShaderProgram::Use() const {
    glUseProgram(ID);
}

void RenderSystemGL::ShaderProgram::Delete() {
    if (ID) {
        glDeleteProgram(ID);
        ID = 0;
    }
}

bool RenderSystemGL::ShaderProgram::CheckCompileErrors(unsigned int shader, const char* type) {
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

bool RenderSystemGL::ShaderProgram::CheckLinkErrors(unsigned int program) {
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

RenderSystemGL::VertexBuffer::VertexBuffer() {
    glGenBuffers(1, &ID);
}
RenderSystemGL::VertexBuffer::~VertexBuffer() {
    if (ID) glDeleteBuffers(1, &ID);
}
void RenderSystemGL::VertexBuffer::Bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, ID);
}
void RenderSystemGL::VertexBuffer::Unbind() const {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void RenderSystemGL::VertexBuffer::SetData(const void* data, size_t size) const {
    glBindBuffer(GL_ARRAY_BUFFER, ID);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

RenderSystemGL::VertexArray::VertexArray() {
    glGenVertexArrays(1, &ID);
}
RenderSystemGL::VertexArray::~VertexArray() {
    if (ID) glDeleteVertexArrays(1, &ID);
}
void RenderSystemGL::VertexArray::Bind() const {
    glBindVertexArray(ID);
}
void RenderSystemGL::VertexArray::Unbind() const {
    glBindVertexArray(0);
}
void RenderSystemGL::VertexArray::AddVertexAttribute(unsigned int index, int size, unsigned int type, bool normalized, size_t stride, const void* pointer) const {
    glVertexAttribPointer(index, size, type, normalized ? GL_TRUE : GL_FALSE, stride, pointer);
    glEnableVertexAttribArray(index);
}

bool RenderSystemGL::Init(void* windowHandle, int width, int height) {
    m_Window = static_cast<SDL_Window*>(windowHandle);
    m_GLContext = SDL_GL_CreateContext(m_Window);
    if (!m_GLContext) {
        std::cerr << "[GL] Failed to create GL context\n";
        return false;
    }
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        std::cerr << "[GL] Failed to load GL functions\n";
        return false;
    }
    SDL_GL_SetSwapInterval(1);
    std::cout << "[GL] OpenGL initialized\n";

    glEnable(GL_DEPTH_TEST);

    m_Shader = new ShaderProgram();
    if (!m_Shader->Compile(vertexShaderSrc, fragmentShaderSrc)) {
        std::cerr << "[GL] Shader compilation failed\n";
        return false;
    }
    return true;
}

void RenderSystemGL::Shutdown() {
    if (m_Shader) {
        m_Shader->Delete();
        delete m_Shader;
        m_Shader = nullptr;
    }
    if (m_GLContext) {
        SDL_GL_DeleteContext(m_GLContext);
        m_GLContext = nullptr;
    }
}

void RenderSystemGL::BeginFrame() {
    glClearColor(0.1f, 0.1f, 0.25f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_CULL_FACE);
}

void RenderSystemGL::EndFrame() {
    SDL_GL_SwapWindow(m_Window);
}

void RenderSystemGL::OnResize(int width, int height) {
    glViewport(0, 0, width, height);
}

// NEW METHOD: Draw any triangle data passed in by client/game
void RenderSystemGL::DrawTriangle(const float* vertexData, size_t size) {
    VertexArray vao;
    VertexBuffer vbo;
    vao.Bind();
    vbo.SetData(vertexData, size);
    vao.AddVertexAttribute(0, 3, GL_FLOAT, false, 6 * sizeof(float), (void*)0);
    vao.AddVertexAttribute(1, 3, GL_FLOAT, false, 6 * sizeof(float), (void*)(3 * sizeof(float)));

    m_Shader->Use();
    Matrix mvp = Matrix::Identity();
    int mvpLoc = glGetUniformLocation(m_Shader->ID, "u_MVP");
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, mvp.Data());

    glDrawArrays(GL_TRIANGLES, 0, 3);

    vao.Unbind();
    vbo.Unbind();
}