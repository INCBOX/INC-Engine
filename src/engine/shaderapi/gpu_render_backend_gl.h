#pragma once

#include "shaderapi/gpu_render_interface.h"
#include "shaderapi/gl_shader_program.h"
#include "shaderapi/igpu_mesh.h"
#include "renderer/istarfieldrenderer.h"
#include "renderer/gl_starfield_renderer.h"
#include "mathlib/matrix4x4_f.h"

#include <SDL.h>
#include <glad/glad.h>
#include <memory>

// Forward declarations
class ShaderProgram;

class GPURenderBackendGL : public IGPURenderInterface {
public:
    bool Init(void* windowHandle, int width, int height) override;
    void Shutdown() override;
	
    void BeginFrame() override;
    void EndFrame() override;
    void OnResize(int width, int height) override;
    void PrepareFrame(int width, int height) override;

    void SetViewMatrix(const Mat4_f& viewMatrix) override;
    void SetProjectionMatrix(const Mat4_f& projMatrix) override;

    void DrawMesh(const IGPUMesh& mesh, const Mat4_f& modelMatrix) override;
	
	// GEOMETRY
	IGPUMesh* CreateMesh() override;
	
	// STARFIELD
    bool LoadStarfieldShaders() override {
        return m_GLStarfieldRenderer->LoadStarfieldShaders();
    }
    void RenderStarfield(float elapsedTime) override {
        m_GLStarfieldRenderer->RenderStarfield(elapsedTime);
    }
    void ReleaseStarfield() override {
        m_GLStarfieldRenderer->ReleaseStarfield();
    }
    void SetDepthTestEnabled(bool enabled) override {
        m_GLStarfieldRenderer->SetDepthTestEnabled(enabled);
    }
    void SetDepthMaskEnabled(bool enabled) override {
        m_GLStarfieldRenderer->SetDepthMaskEnabled(enabled);
    }
	
private:
	const IGPUMesh* m_LastBoundMesh = nullptr; // PERFORMANCE
    SDL_Window* m_Window = nullptr;
    SDL_GLContext m_GLContext = nullptr;

    std::unique_ptr<ShaderProgram> m_Shader;

    GLuint m_ShaderProgram = 0;
    GLint m_TransformUBO = 0;
    GLuint m_UBOHandle = 0;
    int m_MVPLocation = -1;

	void UpdateViewProjectionMatrixIfNeeded();
	void UpdateMVP(const Mat4_f& modelMatrix);

    Mat4_f m_ViewMatrix;
    Mat4_f m_ProjectionMatrix;
    Mat4_f m_ViewProjectionMatrix;
    bool m_MVPDirty = true;
	
    // STARFIELD renderer pointer
    std::unique_ptr<GLStarfieldRenderer> m_GLStarfieldRenderer;
};