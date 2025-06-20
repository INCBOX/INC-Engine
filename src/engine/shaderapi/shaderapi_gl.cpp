#include "shaderapi/shaderapi_gl.h"
#include "shaderapi/shaderapi_gl_shader.h"
#include "shaderapi/shaderapi_gl_buffer.h"
#include "shaderapi/shaderapi_gl_vao.h"
#include "shaderapi/gl_mesh.h"

#include <glad/glad.h>
#include <iostream>
#include "mathlib/matrix.h"

// CreateMesh
IMesh* ShaderAPI_GL::CreateMesh() {
    return new MeshGL();
}

// Init: create GL context, load glad, compile shaders
bool ShaderAPI_GL::Init(void* windowHandle, int /*width*/, int /*height*/) {
    m_Window = static_cast<SDL_Window*>(windowHandle);

    // Set GL context attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    m_GLContext = SDL_GL_CreateContext(m_Window);
    if (!m_GLContext) {
        std::cerr << "[GL] Failed to create GL context\n";
        return false;
    }

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        std::cerr << "[GL] Failed to load GL functions\n";
        return false;
    }

    SDL_GL_SetSwapInterval(0); // Disable vsync for benchmarking Defaukt: (1)
    std::cout << "[GL] OpenGL initialized\n";
	
    // Disable face culling to check if it's the cause of invisible spheres
    glDisable(GL_CULL_FACE);

    // Compile and upload main shader
    m_Shader = std::make_unique<ShaderProgram>();
    if (!m_Shader->CompileFromFile("hl3/shaders/cube.vert", "hl3/shaders/cube.frag")) {
        std::cerr << "[GL] Shader compilation failed\n";
        return false;
    }

    m_ShaderProgram = m_Shader->ID;
    m_MVPLocation = glGetUniformLocation(m_ShaderProgram, "u_MVP");

    glEnable(GL_DEPTH_TEST);

    // Initial MVP state
    m_MVPDirty = true;
    m_ViewMatrix = Matrix::Identity();
    m_ProjectionMatrix = Matrix::Identity();
    m_ViewProjectionMatrix = Matrix::Identity();

    return true;
}

void ShaderAPI_GL::Shutdown() {
    if (m_Shader) {
        m_Shader->Delete();
        m_Shader.reset();
    }
    if (m_GLContext) {
        SDL_GL_DeleteContext(m_GLContext);
        m_GLContext = nullptr;
    }
    m_Window = nullptr;
}

void ShaderAPI_GL::BeginFrame() {
    int w, h;
    SDL_GetWindowSize(m_Window, &w, &h);
    PrepareFrame(w, h);

    if (m_MVPDirty) {
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
        m_MVPDirty = false;
    }

    m_Shader->Use(); // Use shader once for entire frame
}

void ShaderAPI_GL::PrepareFrame(int width, int height) {
    glViewport(0, 0, width, height);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float aspect = static_cast<float>(width) / static_cast<float>(height);
    SetProjectionMatrix(Matrix::Perspective(30.0f, aspect, 0.1f, 100.0f));
}

void ShaderAPI_GL::EndFrame() {
    SDL_GL_SwapWindow(m_Window);
}

void ShaderAPI_GL::OnResize(int width, int height) {
    glViewport(0, 0, width, height);

    float aspect = static_cast<float>(width) / static_cast<float>(height);
    SetProjectionMatrix(Matrix::Perspective(30.0f, aspect, 0.1f, 100.0f));
}

// MVP
void ShaderAPI_GL::SetViewMatrix(const Matrix& viewMatrix) {
    m_ViewMatrix = viewMatrix;
    m_MVPDirty = true;
}

void ShaderAPI_GL::SetProjectionMatrix(const Matrix& projMatrix) {
    m_ProjectionMatrix = projMatrix;
    m_MVPDirty = true;
}

// MESH
void ShaderAPI_GL::DrawMesh(const IMesh& mesh, const Matrix& modelMatrix) {
    if (m_MVPDirty) {
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
        m_MVPDirty = false;
    }

    Matrix mvp = m_ViewProjectionMatrix * modelMatrix;
    glUniformMatrix4fv(m_MVPLocation, 1, GL_FALSE, &mvp[0][0]);

    if (&mesh != m_LastBoundMesh) {
        mesh.Bind();
        m_LastBoundMesh = &mesh;
    }

    glDrawElements(GL_TRIANGLES, mesh.GetIndexCount(), GL_UNSIGNED_INT, nullptr);
}