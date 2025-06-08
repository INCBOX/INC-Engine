#include <iostream>
#include <chrono>

#include "Math.h"
#include "Camera/FPSCamera.h"
#include "Levels/TestLevel_1.h"
#include "Console/Console.h" // ✅ NEW

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define SDL_MAIN_HANDLED // WE ADD THIS TO AVOID MISSING WINMAIN REFERENCE
#include <SDL2/SDL.h>	
#include <windows.h>
#include <filesystem>	// must come after
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

    SDL_Window* window = SDL_CreateWindow("INC-Engine",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
    );

    if (!window) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_GLContext glContext = SDL_GL_CreateContext(window);
if (!glContext) {
    std::cerr << "[Error] Failed to create OpenGL context!" << std::endl;
    return 1;
}

if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
    std::cerr << "[Error] Failed to initialize GLAD!" << std::endl;
    return 1;
}

// ✅ Now it is safe to query OpenGL info
std::cout << "[OpenGL Version] " << glGetString(GL_VERSION) << std::endl;
std::cout << "[Renderer] " << glGetString(GL_RENDERER) << std::endl;


    SDL_SetRelativeMouseMode(SDL_TRUE);
    glViewport(0, 0, 1280, 720);
    glEnable(GL_DEPTH_TEST);

    FPSCamera camera;
	Console console; // ✅ NEW
	
	// console.InitShader(); // ✅ Load console.vert/frag
	// console.InitFont();   // ✅ Load font atlas and metadata
	
    LoadTestLevel_1();

    auto lastTime = std::chrono::high_resolution_clock::now();
    bool running = true;
    SDL_Event event;

    while (running) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;

        while (SDL_PollEvent(&event)) {
        console.ProcessEvent(event);
            if (event.type == SDL_QUIT ||
                (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
                running = false;
            }

			// CONSOLE
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_BACKQUOTE) {
                console.Toggle(); // ✅ Toggle console with ~
            }
			
            console.ProcessEvent(event); // ✅ Pass all events to console
            if (!console.IsActive()) {
				
			// 🖱️ Mouse movement input for camera rotation
                if (event.type == SDL_MOUSEMOTION) {
                    camera.ProcessMouseMovement((float)event.motion.xrel, (float)event.motion.yrel);
                }
            }
        }
		
        if (!console.IsActive()) {
            const Uint8* keystate = SDL_GetKeyboardState(nullptr);
            camera.ProcessInput(keystate, deltaTime);
        }
		
        // ⌨️ Keyboard movement input
        camera.Update(deltaTime);

        glClearColor(0.1f, 0.05f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Mat4 view = camera.GetViewMatrix();
        Mat4 projection = Mat4::perspective(radians(70.0f), 1280.0f / 720.0f, 0.1f, 100.0f);
        RenderTestLevel_1(view, projection);

        console.Render(1280, 720); // ✅ Draw console overlay

        console.Render(1280, 720);
        SDL_GL_SwapWindow(window);
    }

    UnloadTestLevel_1();
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}