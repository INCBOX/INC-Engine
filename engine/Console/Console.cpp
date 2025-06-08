#define GLAD_GL_IMPLEMENTATION 1
#define GLAD_GL_33_COMPATIBILITY_PROFILE 1

#include "Console.h"
#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

GLuint consoleShader = 0;

// --- Shader loading helpers ---

static std::string ReadFile(const char* path) {
    std::ifstream file(path);
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

static GLuint CompileShader(GLenum type, const std::string& src) {
    GLuint shader = glCreateShader(type);
    const char* c_str = src.c_str();
    glShaderSource(shader, 1, &c_str, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        std::cerr << "Shader compilation error:\n" << log << std::endl;
    }
    return shader;
}

static GLuint LoadShaderProgram(const char* vertexPath, const char* fragmentPath) {
    std::string vertSrc = ReadFile(vertexPath);
    std::string fragSrc = ReadFile(fragmentPath);

    GLuint vertShader = CompileShader(GL_VERTEX_SHADER, vertSrc);
    GLuint fragShader = CompileShader(GL_FRAGMENT_SHADER, fragSrc);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);
    glLinkProgram(program);

    GLint linked;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked) {
        char log[512];
        glGetProgramInfoLog(program, 512, nullptr, log);
        std::cerr << "Shader link error:\n" << log << std::endl;
    }

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
    return program;
}

// --- Console Methods ---

Console::Console() : active(false), historyIndex(-1) {}

void Console::Toggle() {
    active = !active;
    if (active) {
        SDL_StartTextInput();
        std::cout << "[Console] Activated and SDL text input started" << std::endl;
    } else {
        SDL_StopTextInput();
        std::cout << "[Console] Deactivated and SDL text input stopped" << std::endl;
    }
}

bool Console::IsActive() const {
    return active;
}

void Console::ProcessEvent(const SDL_Event& event) {
    if (!active) return;

    if (event.type == SDL_TEXTINPUT) {
        inputBuffer += event.text.text;
        std::cout << "TEXT: " << event.text.text << " -> Buffer: " << inputBuffer << std::endl;
    } else if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_BACKSPACE:
                if (!inputBuffer.empty()) inputBuffer.pop_back();
                break;
            case SDLK_RETURN:
                std::cout << "RETURN key pressed, executing: " << inputBuffer << std::endl;
                ExecuteCommand(inputBuffer);
                commandHistory.push_back(inputBuffer);
                inputBuffer.clear();
                historyIndex = -1;
                break;
            case SDLK_UP:
                if (!commandHistory.empty()) {
                    historyIndex = std::max(0, historyIndex - 1);
                    inputBuffer = commandHistory[historyIndex];
                }
                break;
            case SDLK_DOWN:
                if (!commandHistory.empty() && historyIndex < (int)commandHistory.size() - 1) {
                    historyIndex++;
                    inputBuffer = commandHistory[historyIndex];
                } else {
                    inputBuffer.clear();
                    historyIndex = -1;
                }
                break;
            default:
                break;
        }
    }
}

void Console::ExecuteCommand(const std::string& command) {
    std::cout << "> " << command << std::endl;
    if (command == "quit") exit(0);
    else if (command.rfind("sensitivity ", 0) == 0) {
        std::string value = command.substr(12);
        std::cout << "Setting sensitivity to " << value << std::endl;
    } else {
        std::cout << "Unknown command: " << command << std::endl;
    }
}

void Console::Update(float deltaTime) {
    // No-op for now
}

void Console::Render(int width, int height) {
    static GLuint vao = 0, vbo = 0;
    if (vao == 0) {
        float vertices[] = {
            // positions     // texcoords
            -1.0f,  1.0f,    0.0f, 1.0f,
            -1.0f,  0.5f,    0.0f, 0.0f,
             1.0f,  0.5f,    1.0f, 0.0f,
             1.0f,  1.0f,    1.0f, 1.0f,
        };
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    // Use shader
    if (active) {
    glUseProgram(consoleShader);
glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // set default background color
    GLint colorLoc = glGetUniformLocation(consoleShader, "uColor");
    glUniform4f(colorLoc, 0.0f, 0.0f, 0.0f, 0.7f); // semi-transparent black

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
}
    }