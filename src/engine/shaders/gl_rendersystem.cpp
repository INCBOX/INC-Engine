#include "shaders/gl_rendersystem.h"
#include <glad/glad.h>
#include <iostream>
#include "mathlib/matrix.h"
#include "mathlib/vector.h"

// === Shader sources ===
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

// === ShaderProgram methods ===
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

// === VertexBuffer methods ===
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

// === VertexArray methods ===
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

// === RenderSystemGL methods ===
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

    float vertices[] = {
        -0.5f, -0.5f, 0.f,  1.f, 0.f, 0.f,
         0.5f, -0.5f, 0.f,  0.f, 1.f, 0.f,
         0.0f,  0.5f, 0.f,  0.f, 0.f, 1.f
    };

    m_VAO = new VertexArray();
    m_VBO = new VertexBuffer();

    m_VAO->Bind();
    m_VBO->SetData(vertices, sizeof(vertices));
    m_VAO->AddVertexAttribute(0, 3, GL_FLOAT, false, 6 * sizeof(float), (void*)0);
    m_VAO->AddVertexAttribute(1, 3, GL_FLOAT, false, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    m_VAO->Unbind();
    m_VBO->Unbind();

    // 🔍 Print which vertex attributes are enabled
    GLint maxAttribs;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxAttribs);
    for (int i = 0; i < maxAttribs; ++i) {
        GLint enabled = 0;
        glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &enabled);
        if (enabled) std::cout << "[GL] Vertex attribute " << i << " is enabled\n";
    }

    return true;
}

void RenderSystemGL::Renderer_Frame(int width, int height) {
    glViewport(0, 0, width, height);
    glClearColor(0.1f, 0.1f, 0.25f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_CULL_FACE); // optional, just for now

    m_Shader->Use();

    Matrix mvp = Matrix::Identity(); // still flat 2D for now

    int mvpLoc = glGetUniformLocation(m_Shader->ID, "u_MVP");
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, mvp.Data());

    m_VAO->Bind();
    glDrawArrays(GL_TRIANGLES, 0, 3);
    m_VAO->Unbind();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // make sure it's solid
}

void RenderSystemGL::Shutdown() {
    if (m_Shader) {
        m_Shader->Delete();
        delete m_Shader;
        m_Shader = nullptr;
    }
    if (m_VBO) {
        delete m_VBO;
        m_VBO = nullptr;
    }
    if (m_VAO) {
        delete m_VAO;
        m_VAO = nullptr;
    }

    if (m_GLContext) {
        SDL_GL_DeleteContext(m_GLContext);
        m_GLContext = nullptr;
    }
}

void RenderSystemGL::BeginFrame() {}

void RenderSystemGL::EndFrame() {
    SDL_GL_SwapWindow(m_Window);
}

void RenderSystemGL::OnResize(int width, int height) {
    glViewport(0, 0, width, height);
}