#include "runtime_gamedata_path.h"
#include "mathlib/Mat4.h"
#include "hud.h"
#include "console.h"

#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace mathlib;

extern bool gWireframeMode;
extern float gCameraSpeed;

// === Internal globals for shader & background ===
static GLuint consoleVAO = 0;
static GLuint consoleVBO = 0;
static GLuint consoleShader = 0;

void Console::Toggle() {
    active = !active;
    std::cout << (active ? "Console Activated" : "Console Deactivated") << std::endl;
}

bool Console::IsActive() const {
    return active;
}

// === Shader loading helpers ===
static std::string LoadTextFile(const char* path) {
    std::ifstream f(path);
    if (!f.is_open()) {
        std::cerr << "[ShaderLoader] Failed to load: " << path << std::endl;
        return "";
    }
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
    std::string vs = LoadTextFile(gamedata::Shader("console.vert").c_str());
    std::string fs = LoadTextFile(gamedata::Shader("console.frag").c_str());

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

// === Console Class Implementation ===

Console::Console() : active(false), inputBuffer(""), historyIndex(-1), hudRef(nullptr) {
    if (consoleShader == 0)
        consoleShader = LoadConsoleShader();
}

Console::Console(HUD* hud)
    : active(false), inputBuffer(""), historyIndex(-1), hudRef(hud), blinkTimer(0.0f), blinkVisible(true)
{
    // blinking cursor setup
    if (consoleShader == 0)
        consoleShader = LoadConsoleShader();

// Font is now loaded by BitmapFontRenderer internally.

}

void Console::ProcessEvent(const SDL_Event& event) {
    if (!active) return;

    if (event.type == SDL_TEXTINPUT) {
        // Filter out accidental backtick insert from toggle key
        if (std::string(event.text.text) != "`") {
            inputBuffer += event.text.text;
        }
    } else if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_BACKSPACE && !inputBuffer.empty()) {
            inputBuffer.pop_back();
        } else if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_KP_ENTER) {
            ExecuteCommand(inputBuffer);
            inputBuffer.clear();
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

    } else if (command == "wireframe on") {
        gWireframeMode = true;
        std::cout << "Wireframe: ON" << std::endl;

    } else if (command == "wireframe off") {
        gWireframeMode = false;
        std::cout << "Wireframe: OFF" << std::endl;


	} else if (command.rfind("speed ", 0) == 0) {
		std::string value = command.substr(6);
		try {
			gCameraSpeed = std::stof(value);
			std::cout << "Camera speed set to " << gCameraSpeed << std::endl;
		} catch (...) {
			std::cout << "Invalid speed value." << std::endl;
		}
		

    } else {
        std::cout << "Unknown command: " << command << std::endl;
    }
}

void Console::Update(float deltaTime) {}

void Console::Render(int width, int height) {
    if (!active) return;

    // Setup quad vertices
    float quadVerts[] = {
        0.0f,          0.0f,
        0.0f,          height * 0.25f,
        (float)width,  height * 0.25f,
        (float)width,  0.0f
    };

    if (consoleVAO == 0) {
        glGenVertexArrays(1, &consoleVAO);
        glGenBuffers(1, &consoleVBO);

        glBindVertexArray(consoleVAO);
        glBindBuffer(GL_ARRAY_BUFFER, consoleVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), quadVerts, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glBindVertexArray(0);
    } else {
        glBindBuffer(GL_ARRAY_BUFFER, consoleVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(quadVerts), quadVerts);
    }

    // Use proper shader
    glUseProgram(consoleShader);

    // Setup orthographic projection
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    Mat4 ortho = Mat4::Orthographic(0, (float)width, (float)height, 0, -1.0f, 1.0f);
    GLint loc = glGetUniformLocation(consoleShader, "uProjection");
    glUniformMatrix4fv(loc, 1, GL_FALSE, ortho.Data());

    glBindVertexArray(consoleVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
    glUseProgram(0);

    font.RenderConsoleText(inputBuffer, commandHistory, width, height, blinkVisible);
}