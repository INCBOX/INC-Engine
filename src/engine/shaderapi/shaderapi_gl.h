#pragma once
#include "shaderapi/ishaderapi.h"

#include <SDL.h>
#include <memory>

#include "mathlib/matrix.h"
#include "shaderapi_gl_shader.h"
#include "shaderapi_gl_buffer.h"
#include "shaderapi_gl_vao.h"

// Forward declare Mesh class
class Mesh;

class ShaderAPI_GL : public ShaderAPICore {
public:
    bool Init(void* windowHandle, int width, int height) override;
    void Shutdown() override;
    void BeginFrame() override;
    void Renderer_Frame(int width, int height) override;
    void EndFrame() override;
    void OnResize(int width, int height) override;

    // Draw a mesh with a given model matrix transform
    void DrawMesh(const Mesh& mesh, const Matrix& modelMatrix);

private:
    SDL_Window* m_Window = nullptr;
    SDL_GLContext m_GLContext = nullptr;

    // Removed triangle-specific methods

    void SetMVP(const Matrix& mvp);

    std::unique_ptr<class ShaderProgram> m_Shader;
};