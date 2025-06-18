#pragma once
#include "shaderapi/ishaderapi.h"
#include "shaderapi/shaderapi_gl_shader.h"
#include "shaderapi/imesh.h"
#include "mathlib/matrix.h"
#include <SDL.h>
#include <glad/glad.h>
#include <memory>

// Forward declarations
class ShaderProgram;

class ShaderAPI_GL : public ShaderAPICore {
public:
    bool Init(void* windowHandle, int width, int height) override;
    void Shutdown() override;
    void BeginFrame() override;
    void PrepareFrame(int width, int height) override;
    void EndFrame() override;
    void OnResize(int width, int height) override;

    void SetViewMatrix(const Matrix& viewMatrix) override;
    void SetProjectionMatrix(const Matrix& projMatrix) override;

    void DrawMesh(const IMesh& mesh, const Matrix& modelMatrix) override;

private:
    IMesh* CreateMesh() override;

    SDL_Window* m_Window = nullptr;
    SDL_GLContext m_GLContext = nullptr;

    std::unique_ptr<ShaderProgram> m_Shader;
    GLuint m_ShaderProgram = 0;
    int m_MVPLocation = -1;

    Matrix m_ViewMatrix;
    Matrix m_ProjectionMatrix;
    Matrix m_ViewProjectionMatrix;
    bool m_MVPDirty = true;
};
