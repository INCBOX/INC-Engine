// src/public/shaders/gl_rendersystem.h
#pragma once

#include <SDL.h> // SDL types are needed for context creation
#include <glad/glad.h> // OpenGL specific includes
#include "shaders/irendersystem.h" // Include the interface it implements
#include <vector> // For std::vector used in m_renderQueue

// Class declarations for internal components (PIMPL or nested)
class RenderSystemGL : public IRenderSystem { // Inherit from IRenderSystem
public:
    RenderSystemGL();
    virtual ~RenderSystemGL(); // Make destructor virtual

    // Implement IRenderSystem methods
    bool Init(void* windowHandle, int width, int height) override;
    void BeginFrame() override;
    void RenderFrame(int width, int height) override; // Corrected name
    void SubmitObject(const RenderableObjectData& object) override;
    void EndFrame() override;
    void Shutdown() override;
    void OnResize(int width, int height) override;

private:
    // Internal OpenGL-specific classes
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
        unsigned int ID;
        VertexBuffer();
        ~VertexBuffer();
        void Bind() const;
        void Unbind() const;
        void SetData(const void* data, size_t size) const;
    };

    class VertexArray {
    public:
        unsigned int ID;
        VertexArray();
        ~VertexArray();
        void Bind() const;
        void Unbind() const;
        void AddVertexAttribute(unsigned int index, int size, unsigned int type, bool normalized, size_t stride, const void* pointer) const;
    };

    SDL_Window* m_Window = nullptr;
    SDL_GLContext m_GLContext = nullptr;
    ShaderProgram* m_Shader = nullptr;
    VertexArray* m_VAO = nullptr;
    VertexBuffer* m_VBO = nullptr;

    std::vector<RenderableObjectData> m_renderQueue;
};