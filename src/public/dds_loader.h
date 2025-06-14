#pragma once

#include <cstdint>      // For uint8_t
#include <vector>       // For std::vector
#include <glad/glad.h>  // for GLenum declared


bool LoadDDS(const char* filename, int& width, int& height, GLenum& format, std::vector<uint8_t>& outData);