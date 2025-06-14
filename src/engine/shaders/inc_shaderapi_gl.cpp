// --- src/engine/shaders/inc_shader_gl.cpp ---
// OpenGL backend implementation of the IShaderAPI interface.
// Responsible for compiling shaders, uploading geometry, managing textures, and drawing.
// This file is part of a modular rendering backend system inspired by Source Engine's modularity.

// TODO: Extend with error handling, support for more uniform types, and debug output.

// DDS: The function ShaderAPIGL::LoadDDSTexture now just calls your modular LoadDDSTextureGL function from inc_dds_gl.h.

#include "shaderapi.h"
#include "filesystem_stdio.h" 	// For FS_ResolvePath to find shader files on disk
#include "models/imdl_cpu.h" 	// CPU raw geometry + mesh data
#include "models/imdl_gpu.h" 	// GPU buffers, index counts

#include <glad/glad.h>       // OpenGL loader
#include "inc_dds_gl.h"     // DDS loading utility for OpenGL
#include <fstream>
#include <sstream>
#include <iostream>

namespace INC {

class ShaderAPIGL : public IShaderAPI {
public:
    uint32_t LoadShader(const std::string& vertexRelPath, const std::string& fragRelPath) override {
        std::string vsPath = FS_ResolvePath(vertexRelPath);
        std::string fsPath = FS_ResolvePath(fragRelPath);

        std::ifstream vsFile(vsPath), fsFile(fsPath);
        if (!vsFile.is_open() || !fsFile.is_open()) {
            std::cerr << "[ShaderGL] Failed to open shaders: " << vertexRelPath << ", " << fragRelPath << "\n";
            return 0;
        }

        std::stringstream vss, fss;
        vss << vsFile.rdbuf();
        fss << fsFile.rdbuf();

        return Compile(vss.str(), fss.str());
    }

    void UseShader(uint32_t programID) override {
        glUseProgram(programID);
        CheckGLError("glUseProgram");
    }

    void SetUniform1f(uint32_t program, const std::string& name, float value) override {
        GLint loc = glGetUniformLocation(program, name.c_str());
        if (loc != -1) glUniform1f(loc, value);
    }

    void SetUniform1i(uint32_t program, const std::string& name, int value) override {
        GLint loc = glGetUniformLocation(program, name.c_str());
        if (loc != -1) glUniform1i(loc, value);
    }

    void UploadGeometry(IMDL_GPU& out, const IMDL_CPU& data) override {
        glGenVertexArrays(1, &out.vao);
        glGenBuffers(1, &out.vbo);
        glGenBuffers(1, &out.ebo);

        glBindVertexArray(out.vao);

        glBindBuffer(GL_ARRAY_BUFFER, out.vbo);
        glBufferData(GL_ARRAY_BUFFER, data.vertexData.size() * sizeof(float), data.vertexData.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, out.ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.indexData.size() * sizeof(uint32_t), data.indexData.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 3));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 6));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);

        out.indexCount = static_cast<uint32_t>(data.indexData.size());
    }

    void DrawModel(const IMDL_GPU& model) override {
        glBindVertexArray(model.vao);
        glDrawElements(GL_TRIANGLES, model.indexCount, GL_UNSIGNED_INT, nullptr);
        CheckGLError("glDrawElements");
    }

    uint32_t LoadDDSTexture(const void* data, size_t size, int width, int height, uint32_t ddsFormat) override {
        return LoadDDSTextureGL(data, size, width, height, ddsFormat);
    }

    void BindTexture(uint32_t textureID, int slot = 0) override {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, textureID);
        CheckGLError("glBindTexture slot " + std::to_string(slot));
    }

private:
    uint32_t Compile(const std::string& vs, const std::string& fs) {
        GLuint v = glCreateShader(GL_VERTEX_SHADER);
        const char* vc = vs.c_str();
        glShaderSource(v, 1, &vc, nullptr);
        glCompileShader(v);
        if (!CheckShader(v, "vertex")) {
            glDeleteShader(v);
            return 0;
        }

        GLuint f = glCreateShader(GL_FRAGMENT_SHADER);
        const char* fc = fs.c_str();
        glShaderSource(f, 1, &fc, nullptr);
        glCompileShader(f);
        if (!CheckShader(f, "fragment")) {
            glDeleteShader(v);
            glDeleteShader(f);
            return 0;
        }

        GLuint p = glCreateProgram();
        glAttachShader(p, v);
        glAttachShader(p, f);
        glLinkProgram(p);

        int success;
        glGetProgramiv(p, GL_LINK_STATUS, &success);
        if (!success) {
            char log[512];
            glGetProgramInfoLog(p, 512, nullptr, log);
            std::cerr << "[ShaderGL] Link error:\n" << log << "\n";
            glDeleteProgram(p);
            p = 0;
        }

        glDeleteShader(v);
        glDeleteShader(f);
        return p;
    }

    bool CheckShader(GLuint shader, const char* stage) {
        int success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char log[512];
            glGetShaderInfoLog(shader, 512, nullptr, log);
            std::cerr << "[ShaderGL] " << stage << " shader error:\n" << log << "\n";
            return false;
        }
        return true;
    }

    void CheckGLError(const std::string& location) {
        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            std::cerr << "[ShaderGL] OpenGL error at " << location << ": 0x" << std::hex << err << std::dec << "\n";
        }
    }
};

} // namespace INC