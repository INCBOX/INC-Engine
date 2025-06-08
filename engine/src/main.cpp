#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <iostream>
#include <chrono>

#include "Levels/TestLevel.h"
#include "Camera/Camera.h"
#include "Math.h"   // Your own math header replacing glm

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

    // Use your own Vector3 class here
    Camera camera(Vec3(0.0f, 2.0f, 5.0f), Vec3(0.0f, 1.0f, 0.0f));
    LoadTestLevel();

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
            } else if (event.type == SDL_MOUSEMOTION) {
                camera.ProcessMouseMovement(event.motion.xrel, event.motion.yrel);
            }
        }

        const Uint8* keystate = SDL_GetKeyboardState(nullptr);
			if (keystate[SDL_SCANCODE_W])
				camera.ProcessKeyboard(FORWARD, deltaTime);
			if (keystate[SDL_SCANCODE_S])
				camera.ProcessKeyboard(BACKWARD, deltaTime);
			if (keystate[SDL_SCANCODE_A])
				camera.ProcessKeyboard(LEFT, deltaTime);
			if (keystate[SDL_SCANCODE_D])
				camera.ProcessKeyboard(RIGHT, deltaTime);

		enum Camera_Movement {
			FORWARD,
			BACKWARD,
			LEFT,
			RIGHT
		};


        glClearColor(0.1f, 0.05f, 0.15f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		RenderTestLevel();
		SDL_GL_SwapWindow(window); // platform-specific

        // Use your own Mat4 class and math functions instead of glm
        Mat4 view = camera.GetViewMatrix();
        Mat4 projection = Mat4::perspective(radians(70.0f), 1280.0f / 720.0f, 0.1f, 100.0f);
    }

    CleanupTestLevel();
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
