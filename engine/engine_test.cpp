
#include <iostream>
#include <chrono>
#include <fstream>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <windows.h>
#include <glad/glad.h>

#include "mathlib/Mat4.h"
#include "fps_camera.h"

// External level functions
bool Level_DebugFPS_Init();
void Level_DebugFPS_Update(float dt, const Uint8* keys, int mouseDX, int mouseDY);
void Level_DebugFPS_Render(int screenW, int screenH);
void Level_DebugFPS_Shutdown();

int main(int argc, char* argv[]) {
    std::ofstream errorLog("engine_error.log", std::ios::out);
    #define LOG_ERROR(msg) do { std::cerr << msg << std::endl; errorLog << msg << std::endl; } while(0)

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        LOG_ERROR("SDL_Init Error: " << SDL_GetError());
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    SDL_Window* window = SDL_CreateWindow("INC-Engine FPS Test",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (!window) {
        LOG_ERROR("SDL_CreateWindow Error: " << SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
        LOG_ERROR("Failed to create OpenGL context!");
        return 1;
    }

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        LOG_ERROR("Failed to initialize GLAD");
        return 1;
    }

    SDL_SetRelativeMouseMode(SDL_TRUE);
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, 1280, 720);

    Level_DebugFPS_Init();

    bool running = true;
    SDL_Event event;
    auto lastTime = std::chrono::high_resolution_clock::now();
    int mouseDX = 0, mouseDY = 0;

    while (running) {
        mouseDX = 0;
        mouseDY = 0;

        auto now = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(now - lastTime).count();
        lastTime = now;
        if (deltaTime < 0.0001f) deltaTime = 0.0001f;
        if (deltaTime > 0.2f) deltaTime = 0.016f;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || 
                (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
                running = false;
            }
            if (event.type == SDL_MOUSEMOTION) {
                mouseDX = event.motion.xrel;
                mouseDY = event.motion.yrel;
            }
        }

        const Uint8* keys = SDL_GetKeyboardState(nullptr);

        int w, h;
        SDL_GetWindowSize(window, &w, &h);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Level_DebugFPS_Update(deltaTime, keys, mouseDX, mouseDY);
        Level_DebugFPS_Render(w, h);

        SDL_GL_SwapWindow(window);
    }

    Level_DebugFPS_Shutdown();

    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
