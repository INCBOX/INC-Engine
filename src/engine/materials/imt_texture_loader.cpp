// imt_texture_loader.cpp - Load base texture from .imt using modular filesystem and DDS loader

#include "imt_texture_loader.h"
#include "shaderapi.h"
#include "filesystem.h"        // <-- Now using IFileSystem interface
#include "dds_loader.h"        // Custom DDS file loading logic
#include <iostream>
#include <vector>

// External global pointer to the loaded filesystem interface (set by engine.cpp)
extern IFileSystem* g_pFileSystem;

uint32_t LoadTextureFromIMT(const std::string& basetexturePath) {
    // Resolve the full path using the Source-style virtual path (materials/<path>.dds)
    std::string resolved = g_pFileSystem->ResolvePath("materials/" + basetexturePath + ".dds");
    if (resolved.empty()) {
        std::cerr << "[TextureLoader] Failed to resolve: " << basetexturePath << ".dds\n";
        return 0;
    }

    // Variables to receive DDS texture metadata
    std::vector<uint8_t> texData;
    int width = 0, height = 0;
    uint32_t format = 0;

    // Load and decompress DDS texture into texData
    if (!LoadDDS(resolved.c_str(), width, height, format, texData)) {
        std::cerr << "[TextureLoader] Failed to load .dds texture: " << resolved << "\n";
        return 0;
    }

    // Upload the texture to the GPU via the active ShaderAPI
    return INC::g_pShaderAPI->LoadDDSTexture(texData.data(), texData.size(), width, height, format);
}
