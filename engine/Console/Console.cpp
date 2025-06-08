#include "Console.h"
#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

// === Internal globals for shader & background ===
static GLuint consoleVAO = 0;
static GLuint consoleVBO = 0;
static GLuint consoleShader = 0;

// === Shader loading helpers ===
static std::string LoadTextFile(const char* path) {
    std::ifstream f(path);
    std::stringstream s;
    s << f.rdbuf();
    return s.str();
}

static GLuint CompileShader(GLenum type, const std::string& src) {
    GLuint shader = glCreateShader(type);
    const char* cstr = src.c_str();
    glShaderSource(shader, 1, &cstr, nullptr);
    glCompileShader(shader);
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        std::cerr << "Shader compile error: " << log << std::endl;
    }
    return shader;
}

static GLuint LoadConsoleShader() {
    std::string vs = LoadTextFile("resources/shaders/console.vert");
    std::string fs = LoadTextFile("resources/shaders/console.frag");

    GLuint vert = CompileShader(GL_VERTEX_SHADER, vs);
    GLuint frag = CompileShader(GL_FRAGMENT_SHADER, fs);
    GLuint program = glCreateProgram();
    glAttachShader(program, vert);
    glAttachShader(program, frag);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char log[512];
        glGetProgramInfoLog(program, 512, nullptr, log);
        std::cerr << "Shader link error: " << log << std::endl;
    }

    glDeleteShader(vert);
    glDeleteShader(frag);
    return program;
}

Console::Console() : active(false), inputBuffer(""), historyIndex(-1), hudRef(nullptr) {
    if (consoleShader == 0)
        consoleShader = LoadConsoleShader();
}

Console::Console(HUD* hud) : active(false), inputBuffer(""), historyIndex(-1), hudRef(hud) {
    if (consoleShader == 0)
        consoleShader = LoadConsoleShader();
}

void Console::Toggle() {
    active = !active;
    if (active) {
        SDL_StartTextInput();
    } else {
        SDL_StopTextInput();
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
                    if (historyIndex == -1) historyIndex = static_cast<int>(commandHistory.size()) - 1;
                    else if (historyIndex > 0) --historyIndex;
                    inputBuffer = commandHistory[historyIndex];
                }
                break;
            case SDLK_DOWN:
                if (!commandHistory.empty()) {
                    if (historyIndex != -1 && historyIndex < static_cast<int>(commandHistory.size()) - 1)
                        ++historyIndex;
                    inputBuffer = commandHistory[historyIndex];
                }
                break;
        }
    }
}

void Console::ExecuteCommand(const std::string& command) {
    std::cout << "> " << command << std::endl;
    if (command == "quit") {
        exit(0);
    } else if (command.rfind("sensitivity ", 0) == 0) {
        std::string value = command.substr(12);
        std::cout << "Setting sensitivity to " << value << std::endl;
    } else {
        std::cout << "Unknown command: " << command << std::endl;
    }
}

void Console::Update(float deltaTime) {
    // optional future behavior
}

void Console::Render(int width, int height) {
    if (!active) return;

    // Initialize VAO/VBO for background quad
    if (consoleVAO == 0) {
        float quadVerts[] = {
            -1.0f,  1.0f,
            -1.0f,  0.75f,
             1.0f,  0.75f,
             1.0f,  1.0f
        };

        glGenVertexArrays(1, &consoleVAO);
        glGenBuffers(1, &consoleVBO);

        glBindVertexArray(consoleVAO);
        glBindBuffer(GL_ARRAY_BUFFER, consoleVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), quadVerts, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    glUseProgram(consoleShader);

    // 2D Orthographic matrix (no GLM)
    float ortho[16] = {
        2.0f / width, 0,             0, 0,
        0,           -2.0f / height, 0, 0,
        0,            0,            -1, 0,
        -1,           1,             0, 1
    };

    GLint loc = glGetUniformLocation(consoleShader, "uProjection");
    glUniformMatrix4fv(loc, 1, GL_FALSE, ortho);

    glBindVertexArray(consoleVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);

    // Text overlay
    font.RenderText("> " + inputBuffer, 10, height * 0.22f, width, height);
}
