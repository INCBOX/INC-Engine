// src/engine/gl_rendersystem.cpp
#include "shaders/gl_rendersystem.h"
#include <glad/glad.h>
#include <iostream>
#include <vector> // Required for std::vector in m_renderQueue
#include "mathlib/matrix.h"
#include "mathlib/vector.h"

// === Shader sources === (Your existing shaders with colors)
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
    if (!CheckCompileErrors(fragmentShader, "FRAGMENT")) return false;

    ID = glCreateProgram();
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    glLinkProgram(ID);
    if (!CheckLinkErrors(ID)) return false;

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
    char infoLog[1024];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
        std::cerr << "[GL::Shader] ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\\n" << infoLog << "\\n -- --------------------------------------------------- -- \\n";
        return false;
    }
    return true;
}

bool RenderSystemGL::ShaderProgram::CheckLinkErrors(unsigned int program) {
    int success;
    char infoLog[1024];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 1024, nullptr, infoLog);
        std::cerr << "[GL::Program] ERROR::PROGRAM_LINKING_ERROR\\n" << infoLog << "\\n -- --------------------------------------------------- -- \\n";
        return false;
    }
    return true;
}

// === VertexBuffer methods ===
RenderSystemGL::VertexBuffer::VertexBuffer() {
    glGenBuffers(1, &ID);
}

RenderSystemGL::VertexBuffer::~VertexBuffer() {
    if (ID) {
        glDeleteBuffers(1, &ID);
        ID = 0;
    }
}

void RenderSystemGL::VertexBuffer::Bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, ID);
}

void RenderSystemGL::VertexBuffer::Unbind() const {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void RenderSystemGL::VertexBuffer::SetData(const void* data, size_t size) const {
    glBindBuffer(GL_ARRAY_BUFFER, ID); // Ensure bound before setting data
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW); // <--- CHANGED THIS LINE
    // No unbind here as VAO will bind/unbind VBO.
}

// === VertexArray methods ===
RenderSystemGL::VertexArray::VertexArray() {
    glGenVertexArrays(1, &ID);
}

RenderSystemGL::VertexArray::~VertexArray() {
    if (ID) {
        glDeleteVertexArrays(1, &ID);
        ID = 0;
    }
}

void RenderSystemGL::VertexArray::Bind() const {
    glBindVertexArray(ID);
}

void RenderSystemGL::VertexArray::Unbind() const {
    glBindVertexArray(0);
}

void RenderSystemGL::VertexArray::AddVertexAttribute(unsigned int index, int size, unsigned int type, bool normalized, size_t stride, const void* pointer) const {
    // Note: VAO must be bound BEFORE calling this for it to record the attribute setup
    glVertexAttribPointer(index, size, type, normalized, (GLsizei)stride, pointer);
    glEnableVertexAttribArray(index);
}

// === RenderSystemGL methods ===
RenderSystemGL::RenderSystemGL() {}

RenderSystemGL::~RenderSystemGL() {}

bool RenderSystemGL::Init(void* windowHandle, int width, int height) {
    m_Window = static_cast<SDL_Window*>(windowHandle);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    m_GLContext = SDL_GL_CreateContext(m_Window);
    if (!m_GLContext) {
        std::cerr << "[GL] Failed to create OpenGL context: " << SDL_GetError() << "\\n";
        return false;
    }

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        std::cerr << "[GL] Failed to initialize GLAD\\n";
        return false;
    }

    std::cout << "[GL] OpenGL context created successfully.\\n";
    std::cout << "[GL] Vendor: " << glGetString(GL_VENDOR) << "\\n";
    std::cout << "[GL] Renderer: " << glGetString(GL_RENDERER) << "\\n";
    std::cout << "[GL] Version: " << glGetString(GL_VERSION) << "\\n";

    glEnable(GL_DEPTH_TEST);

    m_Shader = new ShaderProgram();
    if (!m_Shader->Compile(vertexShaderSrc, fragmentShaderSrc)) {
        std::cerr << "[GL] Failed to compile shaders\\n";
        return false;
    }

    m_VAO = new VertexArray();
    m_VBO = new VertexBuffer();

    // --- CORRECTED: Setup VAO and VBO attributes ONCE here ---
    m_VAO->Bind(); // Bind VAO first to record state
    m_VBO->Bind(); // Bind VBO

    // Define vertex attributes ONCE: Position (location 0) and Color (location 1)
    // Stride is 6 floats (3 pos + 3 color)
    m_VAO->AddVertexAttribute(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    m_VAO->AddVertexAttribute(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

    m_VBO->Unbind(); // Unbind VBO
    m_VAO->Unbind(); // Unbind VAO (important: unbind VAO last)
    // ----------------------------------------------------------

    // Debug output
    GLint maxAttribs;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxAttribs);
    std::cout << "[GL] Max vertex attributes: " << maxAttribs << "\\n";
    for (int i = 0; i < maxAttribs; ++i) {
        GLint enabled = 0;
        glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &enabled);
        if (enabled) std::cout << "[GL] Vertex attribute " << i << " is enabled\\n";
    }

    return true;
}

void RenderSystemGL::BeginFrame() {
    // Clear the render queue at the beginning of the frame
    m_renderQueue.clear();
}

void RenderSystemGL::SubmitObject(const RenderableObjectData& object) {
    // In a more complex engine, you'd manage batched rendering here,
    // or store unique VAOs/VBOs per object. For now, just queue the data.
    m_renderQueue.push_back(object);
}

void RenderSystemGL::RenderFrame(int width, int height) {
    glViewport(0, 0, width, height);
    glClearColor(0.1f, 0.1f, 0.25f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_CULL_FACE); // optional, just for now

    m_Shader->Use();

    Matrix mvp = Matrix::Identity(); // still flat 2D for now

    int mvpLoc = glGetUniformLocation(m_Shader->ID, "u_MVP");
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, mvp.Data());

    // --- CORRECTED: Iterate and draw, but DO NOT re-setup attributes ---
    // For simplicity, we are still using a single VAO/VBO for all objects.
    // This means all objects must conform to the same vertex format.
    // If objects have different formats, they need their own VAOs/VBOs,
    // or you need a more advanced rendering system.

    m_VAO->Bind(); // Bind the VAO once for all draws in this frame

    for (const auto& obj : m_renderQueue) {
        // Upload data for the current object.
        // NOTE: This re-uploads data to the *same* VBO every time.
        // For multiple objects, this is inefficient. Consider
        // instancing, batching, or separate VBOs for performance.
        m_VBO->Bind(); // Re-bind VBO if it was unbound
        m_VBO->SetData(obj.vertices, obj.vertexCount * sizeof(float));
        m_VBO->Unbind(); // Unbind VBO


        glDrawArrays(GL_TRIANGLES, 0, obj.vertexCount / 6); // Draw using submitted object's vertex count
    }
    m_VAO->Unbind(); // Unbind VAO after drawing all objects
    // -----------------------------------------------------------------

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // make sure it's solid
}

void RenderSystemGL::EndFrame() {
    SDL_GL_SwapWindow(m_Window);
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
    std::cout << "[GL] Shutdown complete\\n";
}

void RenderSystemGL::OnResize(int width, int height) {
    glViewport(0, 0, width, height);
}