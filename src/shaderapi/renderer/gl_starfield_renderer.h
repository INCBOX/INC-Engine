#pragma once
#include <memory>
#include "shaderapi/gl_shader_program.h"
#include "renderer/istarfieldrenderer.h"

class GLStarfieldRenderer : public IStarfieldRenderer {
public:
    bool LoadStarfieldShaders() override;
    void RenderStarfield(float elapsedTime) override;
    void ReleaseStarfield() override;

    void SetDepthTestEnabled(bool enabled) override;
    void SetDepthMaskEnabled(bool enabled) override;

private:
    void InitStarfieldGeometry();
    void CleanupStarfieldGeometry();

    std::unique_ptr<ShaderProgram> m_StarfieldShader;
    unsigned int m_StarfieldVAO = 0;
    unsigned int m_StarfieldVBO = 0;
};
