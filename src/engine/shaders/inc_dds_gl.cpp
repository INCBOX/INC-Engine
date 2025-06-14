#include "inc_dds_gl.h"
#include <glad/glad.h>
#include <iostream>

// Note: Your DDS loader should parse the DDS header and extract mipmap levels, formats, etc.
// This example assumes 'data' points to the raw compressed texture data after the header.

// Loads a DDS texture into OpenGL and creates a GL texture object.
// Currently, uploads only the top mipmap level (level 0).
// You will want to extend this to upload all mipmaps if present.
// Parameters:
// - data: pointer to compressed texture data (DDS pixel data after header)
// - size: size in bytes of the compressed data for level 0
// - width/height: dimensions of the texture
// - ddsFormat: OpenGL compressed internal format, e.g. GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
uint32_t LoadDDSTextureGL(const void* data, size_t size, int width, int height, uint32_t ddsFormat) {
    GLuint tex = 0;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    // Upload compressed texture data for level 0
    glCompressedTexImage2D(GL_TEXTURE_2D, 0, ddsFormat, width, height, 0, size, data);

    // Set default texture sampling parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Generate mipmaps automatically (only valid if full mip chain is provided or generated)
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);  // Unbind texture
    return tex;
}

// Deletes a previously created OpenGL texture by ID
void DeleteDDSTextureGL(uint32_t textureID) {
    GLuint tex = textureID;
    glDeleteTextures(1, &tex);
}
