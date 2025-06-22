#pragma once
#include <memory>
#include "shaderapi/shaderapi_gl_shader.h"
#include "shaderapi/istarfieldrenderer.h"

class StarfieldRenderer : public IStarfieldRenderer {
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
