#include "shaderapi/shaderapi_gl.h"
#include "shaderapi/shaderapi_gl_shader.h"
#include "shaderapi/shaderapi_gl_buffer.h"
#include "shaderapi/shaderapi_gl_vao.h"
#include "shaderapi/geometry_shader_gl.h"

#include <glad/glad.h>
#include <iostream>
#include "mathlib/matrix4x4_f.h"

// CreateMesh
IGeometry* ShaderAPI_GL::CreateMesh() {
    return new GeometryGL();
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
    glDisable(GL_CULL_FACE); // FOR DEBUG MESH N SHIT, REMOVE LATER..

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
    m_ViewMatrix = Mat4_f::Identity();
    m_ProjectionMatrix = Mat4_f::Identity();
    m_ViewProjectionMatrix = Mat4_f::Identity();
	
    // --- STARFIELD STARTS HERE ---
    if (!LoadStarfieldShaders()) {
        std::cerr << "[GL] Failed to load starfield shaders\n";
        return false;
    }

    return true;
}

void ShaderAPI_GL::Shutdown() {
    if (m_Shader) {
        m_Shader->Delete();
        m_Shader.reset();
    }
	
	ReleaseStarfield();  // STARFIELD CLEANUP!
	
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

    UpdateViewProjectionMatrixIfNeeded();

    m_Shader->Use(); // Bind shader once per frame
}

void ShaderAPI_GL::PrepareFrame(int width, int height) {
    glViewport(0, 0, width, height);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float aspect = static_cast<float>(width) / static_cast<float>(height);
    SetProjectionMatrix(Mat4_f::Perspective(30.0f, aspect, 0.1f, 100.0f));
}

void ShaderAPI_GL::EndFrame() {
    SDL_GL_SwapWindow(m_Window);
}

void ShaderAPI_GL::OnResize(int width, int height) {
    glViewport(0, 0, width, height);

    float aspect = static_cast<float>(width) / static_cast<float>(height);
    SetProjectionMatrix(Mat4_f::Perspective(30.0f, aspect, 0.1f, 100.0f));
}

// MVP
void ShaderAPI_GL::SetViewMatrix(const Mat4_f& viewMatrix) {
    m_ViewMatrix = viewMatrix;
    m_MVPDirty = true;
}

void ShaderAPI_GL::SetProjectionMatrix(const Mat4_f& projMatrix) {
    m_ProjectionMatrix = projMatrix;
    m_MVPDirty = true;
}

// MESH
void ShaderAPI_GL::DrawMesh(const IGeometry& mesh, const Mat4_f& modelMatrix) {
    UpdateMVP(modelMatrix); // Centralized MVP upload

    if (&mesh != m_LastBoundMesh) {
        mesh.Bind();
        m_LastBoundMesh = &mesh;
    }

    glDrawElements(GL_TRIANGLES, mesh.GetIndexCount(), GL_UNSIGNED_INT, nullptr);
}

// PRIVATE HELPER: Recalculate the combined ViewProjection matrix if dirty
void ShaderAPI_GL::UpdateViewProjectionMatrixIfNeeded() {
    if (m_MVPDirty) {
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
        m_MVPDirty = false;
    }
}

// CENTRALIZED MVP
void ShaderAPI_GL::UpdateMVP(const Mat4_f& modelMatrix) {
    Mat4_f mvp = m_ViewProjectionMatrix * modelMatrix;
    glUniformMatrix4fv(m_MVPLocation, 1, GL_FALSE, &mvp[0][0]);
}



// STARFIELD
bool ShaderAPI_GL::LoadStarfieldShaders() {
    m_StarfieldShader = std::make_unique<ShaderProgram>();
    if (!m_StarfieldShader->CompileFromFile("hl3/shaders/starfield.vert", "hl3/shaders/starfield.frag")) {
        std::cerr << "[GL] Starfield shader compilation failed\n";
        return false;
    }

    InitStarfieldGeometry();
    return true;
}
// STARFIELD
void ShaderAPI_GL::InitStarfieldGeometry() {
    // Fullscreen quad (2 triangles)
    float vertices[] = {
        // positions   // texCoords
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f,

        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f,
        -1.0f,  1.0f,  0.0f, 1.0f
    };

    glGenVertexArrays(1, &m_StarfieldVAO);
    glGenBuffers(1, &m_StarfieldVBO);

    glBindVertexArray(m_StarfieldVAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_StarfieldVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); // position
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1); // texCoords
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindVertexArray(0);
}
// STARFIELD
void ShaderAPI_GL::RenderStarfield(float elapsedTime) {
    m_StarfieldShader->Use();

    int timeLocation = glGetUniformLocation(m_StarfieldShader->ID, "u_Time");
    glUniform1f(timeLocation, elapsedTime);

    glBindVertexArray(m_StarfieldVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
// STARFIELD
void ShaderAPI_GL::ReleaseStarfield() {
    if (m_StarfieldShader) {
        m_StarfieldShader->Delete();
        m_StarfieldShader.reset();
    }

    CleanupStarfieldGeometry();
}
// STARFIELD
void ShaderAPI_GL::CleanupStarfieldGeometry() {
    if (m_StarfieldVBO) {
        glDeleteBuffers(1, &m_StarfieldVBO);
        m_StarfieldVBO = 0;
    }
    if (m_StarfieldVAO) {
        glDeleteVertexArrays(1, &m_StarfieldVAO);
        m_StarfieldVAO = 0;
    }
}
// STARFIELD