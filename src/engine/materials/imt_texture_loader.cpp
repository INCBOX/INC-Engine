// texture_loader.cpp - Load base texture from .imt using stb_image
#include <string>
#include <iostream>
#include <glad/glad.h>
#include "stb_image.h"
#include "filesystem_stdio.h"

unsigned int LoadTextureFromIMT(const std::string& basetexturePath) {
    std::string resolved = FS_ResolvePath("materials/" + basetexturePath + ".png");
    if (resolved.empty()) {
        std::cerr << "[TextureLoader] Failed to resolve: " << basetexturePath << ".png\n";
        return 0;
    }

    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(resolved.c_str(), &width, &height, &channels, 4);

    if (!data) {
        std::cerr << "[TextureLoader] Failed to load image: " << resolved << "\n";
        return 0;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);

    std::cout << "[TextureLoader] Loaded texture: " << resolved << " (" << width << "x" << height << ")\n";
    return textureID;
}
