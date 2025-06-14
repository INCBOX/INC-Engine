#include "dds_loader.h"
#include <fstream>
#include <cstring>
#include <vector>
#include <glad/glad.h>  // Or the OpenGL header you use

// DDS constants
#define FOURCC_DXT1 0x31545844  // "DXT1"
#define FOURCC_DXT3 0x33545844  // "DXT3"
#define FOURCC_DXT5 0x35545844  // "DXT5"

bool LoadDDS(const std::string& filepath, int& width, int& height, int& mipCount, uint32_t& glFormat, std::vector<uint8_t>& outData)
{
    std::ifstream file(filepath, std::ios::binary);
    if (!file) return false;

    char magic[4];
    file.read(magic, 4);
    if (std::strncmp(magic, "DDS ", 4) != 0) return false;

    uint8_t header[124];
    file.read(reinterpret_cast<char*>(header), 124);

    height = *reinterpret_cast<uint32_t*>(&header[8]);
    width  = *reinterpret_cast<uint32_t*>(&header[12]);
    uint32_t linearSize = *reinterpret_cast<uint32_t*>(&header[16]);
    mipCount = *reinterpret_cast<uint32_t*>(&header[24]);
    uint32_t fourCC = *reinterpret_cast<uint32_t*>(&header[80]);

    switch (fourCC) {
        case FOURCC_DXT1: glFormat = 0x83F1; break; // GL_COMPRESSED_RGBA_S3TC_DXT1_EXT
        case FOURCC_DXT3: glFormat = 0x83F2; break; // GL_COMPRESSED_RGBA_S3TC_DXT3_EXT
        case FOURCC_DXT5: glFormat = 0x83F3; break; // GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
        default: return false;
    }

    size_t blockSize = (fourCC == FOURCC_DXT1) ? 8 : 16;
    size_t bufferSize = (mipCount > 1) ? linearSize * 2 : linearSize;

    outData.resize(bufferSize);
    file.read(reinterpret_cast<char*>(outData.data()), bufferSize);

    return true;
}
