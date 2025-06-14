#include "shaderapi.h"
#include "models/imdl_cpu.h"
#include "models/imdl_gpu.h"
#include <iostream>

namespace INC {

class ShaderAPIVK : public IShaderAPI {
public:
    ~ShaderAPIVK() override = default;

    uint32_t LoadShader(const std::string& vertexRelPath, const std::string& fragRelPath) override {
        std::cout << "[ShaderAPIVK] (stub) LoadShader: " << vertexRelPath << ", " << fragRelPath << "\n";
        return 1; // dummy program ID
    }

    void UseShader(uint32_t programID) override {
        std::cout << "[ShaderAPIVK] (stub) UseShader: " << programID << "\n";
    }

    void SetUniform1f(uint32_t /*program*/, const std::string& name, float value) override {
        std::cout << "[ShaderAPIVK] (stub) SetUniform1f: " << name << " = " << value << "\n";
    }

    void SetUniform1i(uint32_t /*program*/, const std::string& name, int value) override {
        std::cout << "[ShaderAPIVK] (stub) SetUniform1i: " << name << " = " << value << "\n";
    }

    void UploadGeometry(IMDL_GPU& out, const IMDL_CPU& in) override {
#ifdef USE_VULKAN
        out.vkVertexBuffer = reinterpret_cast<VkBuffer>(0x12345678abcdef00); // fake handle
        out.vkIndexBuffer  = reinterpret_cast<VkBuffer>(0x0fedcba987654321);
#endif
        out.indexCount = static_cast<uint32_t>(in.indexData.size());
        out.materialName = in.material;

        std::cout << "[ShaderAPIVK] (stub) Uploaded geometry: "
                  << out.indexCount << " indices, material '" << out.materialName << "'\n";
    }

    void DrawModel(const IMDL_GPU& model) override {
        std::cout << "[ShaderAPIVK] (stub) Drawing model with " << model.indexCount << " indices\n";
    }

    uint32_t LoadDDSTexture(const void* /*data*/, size_t size, int width, int height, uint32_t ddsFormat) override {
        std::cout << "[ShaderAPIVK] (stub) LoadDDSTexture called with size: " << size
                  << ", width: " << width << ", height: " << height << ", format: " << ddsFormat << "\n";
        return 1; // dummy texture handle
    }

    void BindTexture(uint32_t textureID, int slot = 0) override {
        std::cout << "[ShaderAPIVK] (stub) BindTexture: " << textureID << " to slot " << slot << "\n";
    }
};

} // namespace INC