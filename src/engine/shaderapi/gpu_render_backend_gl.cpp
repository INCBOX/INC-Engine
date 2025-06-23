#include "shaderapi/gpu_render_backend_gl.h"
#include "shaderapi/gl_shader_program.h"
#include "shaderapi/gl_buffer.h"
#include "shaderapi/gl_vertex_array.h"
#include "shaderapi/gl_mesh.h"
#include "shaderapi/igpu_mesh.h"

#include "engine_globals.h" // THIS IS KINDA WRONG...
#include "camera_manager.h"	// THIS IS OKKO

#include <glad/glad.h>
#include <iostream>
#include "mathlib/matrix4x4_f.h"

// CreateMesh
IGPUMesh* GPURenderBackendGL::CreateMesh() {
    return new GLMesh();
}

// Init: create GL context, load glad, compile shaders
bool GPURenderBackendGL::Init(void* windowHandle, int /*width*/, int /*height*/) {
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
    m_ViewMatrix = Matrix4x4_f::Identity();
    m_ProjectionMatrix = Matrix4x4_f::Identity();
    m_ViewProjectionMatrix = Matrix4x4_f::Identity();
	
    // --- STARFIELD STARTS HERE ---
	m_GLStarfieldRenderer = std::make_unique<GLStarfieldRenderer>();
	if (!m_GLStarfieldRenderer->LoadStarfieldShaders()) {
		// handle error
	}

    return true;
}

void GPURenderBackendGL::Shutdown() {
    if (m_Shader) {
        m_Shader->Delete();
        m_Shader.reset();
    }
	
	if (m_GLStarfieldRenderer) {
		m_GLStarfieldRenderer->ReleaseStarfield();
		m_GLStarfieldRenderer.reset();
	}
	
    if (m_GLContext) {
        SDL_GL_DeleteContext(m_GLContext);
        m_GLContext = nullptr;
    }
    m_Window = nullptr;
}

void GPURenderBackendGL::BeginFrame() {
    int w, h;
    SDL_GetWindowSize(m_Window, &w, &h);
    PrepareFrame(w, h);

    UpdateViewProjectionMatrixIfNeeded();

    m_Shader->Use(); // Bind shader once per frame
	SetViewMatrix(g_CameraManager.GetLocalViewMatrix());
	UpdateMVP(Matrix4x4_f::Identity()); // Upload clean MVP for cases with no model (like skybox)
}

void GPURenderBackendGL::PrepareFrame(int width, int height) {
    glViewport(0, 0, width, height);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float aspect = static_cast<float>(width) / static_cast<float>(height);
    SetProjectionMatrix(Matrix4x4_f::Perspective(30.0f, aspect, 0.1f, 100.0f));
	UpdateViewProjectionMatrixIfNeeded();
}

void GPURenderBackendGL::EndFrame() {
    SDL_GL_SwapWindow(m_Window);
}

void GPURenderBackendGL::OnResize(int width, int height) {
    glViewport(0, 0, width, height);

    float aspect = static_cast<float>(width) / static_cast<float>(height);
    SetProjectionMatrix(Matrix4x4_f::Perspective(30.0f, aspect, 0.1f, 100.0f));
	UpdateViewProjectionMatrixIfNeeded();
}

// MVP
void GPURenderBackendGL::SetViewMatrix(const Matrix4x4_f& viewMatrix) {
    m_ViewMatrix = viewMatrix;
    m_MVPDirty = true;
}

void GPURenderBackendGL::SetProjectionMatrix(const Matrix4x4_f& projMatrix) {
    m_ProjectionMatrix = projMatrix;
    m_MVPDirty = true;
}

// MESH
void GPURenderBackendGL::DrawMesh(const IGPUMesh& mesh, const Matrix4x4_f& modelMatrix) {
    m_Shader->Use();  // Ensure mesh shader is active
    UpdateMVP(modelMatrix); // Upload MVP

    if (&mesh != m_LastBoundMesh) {
        mesh.Bind();
        m_LastBoundMesh = &mesh;
    }

    glDrawElements(GL_TRIANGLES, mesh.GetIndexCount(), GL_UNSIGNED_INT, nullptr);
}

// PRIVATE HELPER: Recalculate the combined ViewProjection matrix if dirty
void GPURenderBackendGL::UpdateViewProjectionMatrixIfNeeded() {
    if (m_MVPDirty) {
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
        m_MVPDirty = false;
    }
}

// CENTRALIZED MVP
void GPURenderBackendGL::UpdateMVP(const Matrix4x4_f& modelMatrix) {
    Matrix4x4_f mvp = m_ViewProjectionMatrix * modelMatrix;
    glUniformMatrix4fv(m_MVPLocation, 1, GL_FALSE, &mvp[0][0]);
}