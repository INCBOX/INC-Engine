#include "shaderapi.h"
#include "models/imdl_cpu.h"  // CPU-side geometry
#include "models/imdl_gpu.h"  // GPU-side handles
#include <iostream>

namespace INC {

class ShaderAPIDX : public IShaderAPI {
public:
    ~ShaderAPIDX() override = default;

    uint32_t LoadShader(const std::string& vertexRelPath, const std::string& fragRelPath) override {
        std::cout << "[ShaderAPIDX] (stub) LoadShader: " << vertexRelPath << ", " << fragRelPath << "\n";
        return 1;  // Dummy shader program ID
    }

    void UseShader(uint32_t programID) override {
        std::cout << "[ShaderAPIDX] (stub) UseShader: " << programID << "\n";
    }

    void SetUniform1f(uint32_t program, const std::string& name, float value) override {
        (void)program;
        std::cout << "[ShaderAPIDX] (stub) SetUniform1f: " << name << " = " << value << "\n";
    }

    void SetUniform1i(uint32_t program, const std::string& name, int value) override {
        (void)program;
        std::cout << "[ShaderAPIDX] (stub) SetUniform1i: " << name << " = " << value << "\n";
    }

    void UploadGeometry(IMDL_GPU& out, const IMDL_CPU& in) override {
        // In real code, you'd create DirectX buffers using ID3D11Device.
        out.dxVertexBuffer = nullptr;  // Stub: no real buffer created
        out.dxIndexBuffer = nullptr;
        out.indexCount = static_cast<uint32_t>(in.indexData.size());
        out.materialName = in.material;

        std::cout << "[ShaderAPIDX] (stub) Uploaded geometry: "
                  << out.indexCount << " indices, material '" << out.materialName << "'\n";
    }

    void DrawModel(const IMDL_GPU& model) override {
        std::cout << "[ShaderAPIDX] (stub) Drawing model with " << model.indexCount << " indices\n";
    }

    uint32_t LoadDDSTexture(const void* data, size_t size, int width, int height, uint32_t ddsFormat) override {
        (void)data;
        std::cout << "[ShaderAPIDX] (stub) LoadDDSTexture called with size: " << size
                  << ", width: " << width << ", height: " << height << ", format: " << ddsFormat << "\n";
        return 1;  // Dummy texture ID
    }

    void BindTexture(uint32_t textureID, int slot = 0) override {
        std::cout << "[ShaderAPIDX] (stub) BindTexture: " << textureID << " to slot " << slot << "\n";
    }
};

} // namespace INC