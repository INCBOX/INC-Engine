#include "shaderapi/gpu_render_backend.h"
#include "shaderapi/gpu_render_interface.h"
#include "shaderapi/igpu_mesh.h"

#include <iostream>

class GPUMeshNull : public IGPUMesh {
public:
    void Upload(const std::vector<float>& vertices, const std::vector<unsigned int>& indices) override {}
    void Bind() const override {}
    void Unbind() const override {}
    size_t GetIndexCount() const override { return 0; }
};

class GPURenderBackendNull : public IGPURenderInterface {
public:
    bool Init(void* windowHandle, int width, int height) override { return true; }
    void Shutdown() override {}

    void BeginFrame() override {}
    void EndFrame() override {}
    void OnResize(int width, int height) override {}
    void PrepareFrame(int width, int height) override {}

    void SetViewMatrix(const Matrix4x4_f& viewMatrix) override {}
    void SetProjectionMatrix(const Matrix4x4_f& projMatrix) override {}

    void DrawMesh(const IGPUMesh& mesh, const Matrix4x4_f& modelMatrix) override {}

    IGPUMesh* CreateMesh() override { return new GPUMeshNull(); }

    bool LoadStarfieldShaders() override { return true; }
    void RenderStarfield(float elapsedTime) override {}
    void ReleaseStarfield() override {}
    void SetDepthTestEnabled(bool enabled) override {}
    void SetDepthMaskEnabled(bool enabled) override {}
};

static GPURenderBackendNull* s_pBackendNull = nullptr;

extern "C" {

// Factory function to create the Null backend instance
__declspec(dllexport) IGPURenderInterface* CreateGPUAPI()
{
    printf("[ShaderAPI-Null] CreateGPUAPI called\n");
    if (!s_pBackendNull) {
        s_pBackendNull = new GPURenderBackendNull();
        printf("[ShaderAPI-Null] Created backend instance\n");
    }
    return s_pBackendNull;
}

// Function to destroy the Null backend instance
__declspec(dllexport) void DestroyGPUAPI()
{
    if (s_pBackendNull) {
        s_pBackendNull->Shutdown();
        delete s_pBackendNull;
        s_pBackendNull = nullptr;
    }
}

}