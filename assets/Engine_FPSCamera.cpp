#include <iostream>
#include <chrono>

#include "Math.h"
#include "Camera/FPSCamera.h"
#include "Levels/TestLevel_1.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define SDL_MAIN_HANDLED // WE ADD THIS TO AVOID MISSING WINMAIN REFERENCE
#include <SDL2/SDL.h>	// must come after
#include <glad/glad.h>

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_Window* window = SDL_CreateWindow("INC-Engine Test Level",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
    );

    if (!window) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (!glContext || !gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        std::cerr << "Failed to initialize GL context or GLAD" << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_SetRelativeMouseMode(SDL_TRUE);
    glViewport(0, 0, 1280, 720);
    glEnable(GL_DEPTH_TEST);

    FPSCamera camera;
    LoadTestLevel_1();

    auto lastTime = std::chrono::high_resolution_clock::now();
    bool running = true;
    SDL_Event event;

    while (running) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT ||
                (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
                running = false;
            }
        }
		// CAMERA MOVEMENT INPUT
        const Uint8* keystate = SDL_GetKeyboardState(nullptr);
        camera.ProcessInput(keystate, deltaTime);
        camera.Update(deltaTime);

        glClearColor(0.1f, 0.05f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Mat4 view = camera.GetViewMatrix();
        Mat4 projection = Mat4::perspective(radians(70.0f), 1280.0f / 720.0f, 0.1f, 100.0f);
        RenderTestLevel_1(view, projection);

        SDL_GL_SwapWindow(window);
    }

    UnloadTestLevel_1();
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}