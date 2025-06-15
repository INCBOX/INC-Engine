#include "renderer/renderer.h"
#include <glad/glad.h>
#include <SDL.h>           // for SDL_GL_GetProcAddress
#include <iostream>

// Shader sources (hardcoded minimal shaders)
static const char* vertexShaderSrc = R"glsl(
#version 330 core
layout(location = 0) in vec3 aPos;
void main() {
    gl_Position = vec4(aPos, 1.0);
}
)glsl";

static const char* fragmentShaderSrc = R"glsl(
#version 330 core
out vec4 FragColor;
void main() {
    FragColor = vec4(0.3, 0.7, 1.0, 1.0); // light blue color
}
)glsl";

class RendererInternal {
public:
    GLuint shaderProgram = 0;
    GLuint VAO = 0;

    bool CompileShader(GLuint& shader, GLenum type, const char* src) {
        shader = glCreateShader(type);
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);
        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char log[512];
            glGetShaderInfoLog(shader, 512, nullptr, log);
            std::cerr << "[Renderer] Shader compile error: " << log << "\n";
            return false;
        }
        return true;
    }

    bool CreateShaderProgram() {
        GLuint vertexShader, fragmentShader;
        if (!CompileShader(vertexShader, GL_VERTEX_SHADER, vertexShaderSrc)) return false;
        if (!CompileShader(fragmentShader, GL_FRAGMENT_SHADER, fragmentShaderSrc)) return false;

        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);

        GLint success;
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            char log[512];
            glGetProgramInfoLog(shaderProgram, 512, nullptr, log);
            std::cerr << "[Renderer] Shader link error: " << log << "\n";
            return false;
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return true;
    }

    void SetupTriangle() {
        float vertices[] = {
             0.0f,  0.5f, 0.0f,  // top
            -0.5f, -0.5f, 0.0f,  // bottom left
             0.5f, -0.5f, 0.0f   // bottom right
        };

        GLuint VBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Unbind to avoid accidental modification
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
};

static RendererInternal rendererInternal;

bool Renderer::Init() {
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        std::cerr << "[Renderer] Failed to initialize GLAD\n";
        return false;
    }

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    if (!rendererInternal.CreateShaderProgram()) {
        std::cerr << "[Renderer] Failed to create shader program\n";
        return false;
    }

    rendererInternal.SetupTriangle();

    std::cout << "[Renderer] Init successful\n";
    return true;
}

void Renderer::RenderFrame(int width, int height) {
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(rendererInternal.shaderProgram);
    glBindVertexArray(rendererInternal.VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
    glUseProgram(0);

    // NOTE: SDL_GL_SwapWindow(window) must be called in your main loop after this!
}

void Renderer::Shutdown() {
    if (rendererInternal.VAO) {
        glDeleteVertexArrays(1, &rendererInternal.VAO);
        rendererInternal.VAO = 0;
    }
    if (rendererInternal.shaderProgram) {
        glDeleteProgram(rendererInternal.shaderProgram);
        rendererInternal.shaderProgram = 0;
    }
    std::cout << "[Renderer] Shutdown complete\n";
}