// material_manager.cpp - INC Material Manager
#include <string>
#include <unordered_map>
#include <memory>
#include <iostream>
#include "filesystem/imt_material_loader.h"     // LoadIMT
#include "imt_texture_loader.h"      // LoadTextureFromIMT
#include "shader_loader.h"       // LoadShaderFromFile

struct MaterialGPU {
    std::string shaderName;
    GLuint shaderID = 0;
    GLuint textureID = 0;
};

class MaterialManager {
public:
    bool Load(const std::string& materialPath) {
        if (materials.count(materialPath)) return true; // Already loaded

        Material mat = LoadIMT(materialPath);
        if (mat.shader.empty()) return false;

        auto gpuMat = std::make_shared<MaterialGPU>();
        gpuMat->shaderName = mat.shader;

        // Load shader from disk (can be cached later)
        if (mat.shader == "VertexLit") {
            gpuMat->shaderID = LoadShaderFromFile("shaders/vertexlit.vert", "shaders/vertexlit.frag");
        } else {
            std::cerr << "[MaterialManager] Unknown shader: " << mat.shader << "\n";
            return false;
        }

        auto baseIt = mat.parameters.find("basetexture");
        if (baseIt != mat.parameters.end()) {
            gpuMat->textureID = LoadTextureFromIMT(baseIt->second);
        }

        materials[materialPath] = gpuMat;
        return true;
    }

    std::shared_ptr<MaterialGPU> Get(const std::string& materialPath) {
        if (materials.count(materialPath)) return materials[materialPath];
        return nullptr;
    }

private:
    std::unordered_map<std::string, std::shared_ptr<MaterialGPU>> materials;
};
