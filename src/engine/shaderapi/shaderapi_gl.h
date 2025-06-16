#pragma once
#include "shaderapi/ishaderapi.h"

#include <SDL.h>
#include <memory>

#include "mathlib/matrix.h"
#include "shaderapi_gl_shader.h"
#include "shaderapi_gl_buffer.h"
#include "shaderapi_gl_vao.h"

class ShaderAPI_GL : public ShaderAPICore {
public:
    bool Init(void* windowHandle, int width, int height) override;
    void Shutdown() override;
    void BeginFrame() override;
    void Renderer_Frame(int width, int height) override;
    void EndFrame() override;
    void OnResize(int width, int height) override;

private:
    SDL_Window* m_Window = nullptr;
    SDL_GLContext m_GLContext = nullptr;

    void SetupTriangleGeometry();
    void SetMVP(const Matrix& mvp);
    void DrawTriangle();

    std::unique_ptr<class ShaderProgram> m_Shader;
    std::unique_ptr<class VertexArray> m_VAO;
    std::unique_ptr<class VertexBuffer> m_VBO;
};
