#pragma once
class IStarfieldRenderer {
public:
    virtual ~IStarfieldRenderer() = default;

    virtual bool LoadStarfieldShaders() = 0;
    virtual void RenderStarfield(float elapsedTime) = 0;
    virtual void ReleaseStarfield() = 0;

    virtual void SetDepthTestEnabled(bool enabled) = 0;
    virtual void SetDepthMaskEnabled(bool enabled) = 0;
};