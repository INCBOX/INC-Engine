#include "shaderapi/shaderapi_gl.h"
#include "shaderapi/shaderapi_gl_shader.h"
#include "shaderapi/shaderapi_gl_buffer.h"
#include "shaderapi/shaderapi_gl_vao.h"
#include "shaderapi/mesh.h"
#include <glad/glad.h>
#include <iostream>
#include "mathlib/matrix.h"





bool ShaderAPI_GL::Init(void* windowHandle, int width, int height) {
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

    m_Shader = std::make_unique<ShaderProgram>();
    if (!m_Shader->Compile(vertexShaderSrc, fragmentShaderSrc)) {
        std::cerr << "[GL] Shader compilation failed\n";
        return false;
    }

    // Removed SetupTriangleGeometry and related calls

    // Optional: print enabled vertex attributes for debugging
    GLint maxAttribs;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxAttribs);
    for (int i = 0; i < maxAttribs; ++i) {
        GLint enabled = 0;
        glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &enabled);
        if (enabled) std::cout << "[GL] Vertex attribute " << i << " is enabled\n";
    }

    return true;
}


void ShaderAPI_GL::BeginFrame() {
    // Optional: any per-frame setup before rendering
}

void ShaderAPI_GL::EndFrame() {
    SDL_GL_SwapWindow(m_Window);
}



void ShaderAPI_GL::SetMVP(const Matrix& mvp) {
    int mvpLoc = glGetUniformLocation(m_Shader->ID, "u_MVP");
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, mvp.Data());
}

void ShaderAPI_GL::Renderer_Frame(int width, int height) {
    glViewport(0, 0, width, height);
    glClearColor(0.1f, 0.1f, 0.25f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_CULL_FACE); // optional

    m_Shader->Use();
    SetMVP(Matrix::Identity());

    // Removed DrawTriangle();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // solid fill
}

void ShaderAPI_GL::Shutdown() {
    if (m_Shader) {
        m_Shader->Delete();
        m_Shader.reset();
    }

    // m_VBO and m_VAO no longer members, so do NOT reset here

    if (m_GLContext) {
        SDL_GL_DeleteContext(m_GLContext);
        m_GLContext = nullptr;
    }
}


void ShaderAPI_GL::OnResize(int width, int height) {
    glViewport(0, 0, width, height);
}


// MESH
void ShaderAPI_GL::DrawMesh(const Mesh& mesh, const Matrix& modelMatrix) {
    m_Shader->Use();

    // Compute final MVP matrix (viewProjection * model)
    // For now, assume identity viewProjection; you can expand this later
    Matrix mvp = Matrix::Identity(); 
    mvp = mvp * modelMatrix;

    SetMVP(mvp);

    mesh.Bind();
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.GetIndexCount()), GL_UNSIGNED_INT, nullptr);
    mesh.Unbind();
}