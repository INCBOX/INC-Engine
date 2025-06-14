#ifndef INC_DDS_GL_H
#define INC_DDS_GL_H

#include <cstddef>
#include <cstdint>

// Loads a DDS texture into OpenGL and returns the GL texture ID.
// - data: pointer to DDS file data in memory
// - size: size of data buffer in bytes
// - width, height: dimensions of the texture
// - ddsFormat: OpenGL compressed internal format, e.g. GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
uint32_t LoadDDSTextureGL(const void* data, size_t size, int width, int height, uint32_t ddsFormat);

// Deletes a previously created DDS texture.
void DeleteDDSTextureGL(uint32_t textureID);

#endif // INC_DDS_GL_H
