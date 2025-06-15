#pragma once
#include "shaders/irendersystem.h"
#include <SDL.h>

class RenderSystemGL : public IRenderSystem {
public:
    bool Init(void* windowHandle, int width, int height) override;
    void Shutdown() override;
    void BeginFrame() override;
    void Renderer_Frame(int width, int height) override;
    void EndFrame() override;
    void OnResize(int width, int height) override;

private:
    SDL_Window* m_Window = nullptr;
    SDL_GLContext m_GLContext = nullptr;

    // Nested helper classes for shader and buffers
    class ShaderProgram {
    public:
        unsigned int ID = 0;

        bool Compile(const char* vertexSrc, const char* fragmentSrc);
        void Use() const;
        void Delete();

    private:
        bool CheckCompileErrors(unsigned int shader, const char* type);
        bool CheckLinkErrors(unsigned int program);
    };

    class VertexBuffer {
    public:
        unsigned int ID = 0;

        VertexBuffer();
        ~VertexBuffer();

        void Bind() const;
        void Unbind() const;
        void SetData(const void* data, size_t size) const;
    };

    class VertexArray {
    public:
        unsigned int ID = 0;

        VertexArray();
        ~VertexArray();

        void Bind() const;
        void Unbind() const;
        void AddVertexAttribute(unsigned int index, int size, unsigned int type, bool normalized, size_t stride, const void* pointer) const;
    };

    ShaderProgram* m_Shader = nullptr;
    VertexArray* m_VAO = nullptr;
    VertexBuffer* m_VBO = nullptr;
};
