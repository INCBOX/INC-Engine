#include <iostream>
#include <chrono>

#include "runtime_gamedata_path.h"
#include "math.h"

#include "console.h"
#include "hud.h"
#include "FPSCamera.h"

#include "Levels/LevelManager.h"
#include "Levels/Level_TestScene.h"


#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define SDL_MAIN_HANDLED // WE ADD THIS TO AVOID MISSING WINMAIN REFERENCE
#include <SDL2/SDL.h>	
#include <windows.h>
#include <filesystem>	// must come after
#include <glad/glad.h>
#include <fstream>

bool gWireframeMode = false;

// Global matrices for shader MVP
Mat4 gViewMatrix;
Mat4 gProjMatrix;

int main(int argc, char* argv[]) {
	// Redirect error output to a log file
    std::ofstream errorLog("engine_error.log", std::ios::out);
    #define LOG_ERROR(msg) do { std::cerr << msg << std::endl; errorLog << msg << std::endl; } while(0)

	std::cout << "Wireframe: " << (gWireframeMode ? "ON" : "OFF") << std::endl;
	std::cout << "[RenderMode] " << (gWireframeMode ? "GL_LINE" : "GL_FILL") << std::endl;
	
	
    try {
        std::filesystem::current_path(std::filesystem::path(argv[0]).parent_path());
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to set working directory: " << e.what());
    }

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        LOG_ERROR("SDL_Init Error: " << SDL_GetError());
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_Window* window = SDL_CreateWindow("INC-Engine",
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
        LOG_ERROR("Failed to initialize GLAD!");
        return 1;
    }
	// Now it is safe to query OpenGL info
	std::cout << "[OpenGL Version] " << glGetString(GL_VERSION) << std::endl;
	std::cout << "[Renderer] " << glGetString(GL_RENDERER) << std::endl;

    SDL_SetRelativeMouseMode(SDL_TRUE);
    glViewport(0, 0, 1280, 720);
    glEnable(GL_DEPTH_TEST);

	// CAMERA
	FPSCamera camera(Vec3(1000, 0, -500 + 100));  // 100 units out
	
	std::cout << "Camera Pos: " << camera.Position.x << ", "
							<< camera.Position.y << ", "
							<< camera.Position.z << std::endl;
	
	Vec3 front = camera.GetFront();
std::cout << "Camera Front: " << front.x << ", "
          << front.y << ", "
          << front.z << std::endl;

	
	
	HUD hud;
	hud.Init();
	Console console(&hud);
	
    // Initialize modular level system
    LevelManager::SetLevel(std::make_unique<Level_TestScene>());

    // InitSkybox();

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

			// CONSOLE
			if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_BACKQUOTE) {
				console.Toggle(); // Toggle console with ~
				continue; // ✅ Skip processing the backtick
			}
			
			console.ProcessEvent(event);


            if (!console.IsActive() && event.type == SDL_MOUSEMOTION) {
                camera.ProcessMouseMovement((float)event.motion.xrel, (float)event.motion.yrel);
            }
        }
		
        if (!console.IsActive()) {
            const Uint8* keystate = SDL_GetKeyboardState(nullptr);
            camera.ProcessInput(keystate, deltaTime);
        }
		
        // ⌨️ Keyboard movement input
        camera.Update(deltaTime);

		// --- CLEAR SCREEN ---
		glClearColor(0.15f, 0.02f, 0.02f, 1.0f); // deep red-black, alien/Mars vibe
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glPolygonMode(GL_FRONT_AND_BACK, gWireframeMode ? GL_LINE : GL_FILL);

        // === 3D world ===
        glEnable(GL_DEPTH_TEST);
        Mat4 view = camera.GetViewMatrix();
        Mat4 projection = Mat4::perspective(radians(70.0f), 1280.0f / 720.0f, 0.1f, 50000.0f);
		
        gViewMatrix = view;
        gProjMatrix = projection;
		
		Vec3 camPos = camera.Position;
		std::cout << "[Camera Pos] " << camPos.x << ", " << camPos.y << ", " << camPos.z << std::endl;
		std::cout << "[DEBUG] gViewMatrix[0]: " << gViewMatrix.m[0] << std::endl;
		std::cout << "[DEBUG] gProjMatrix[0]: " << gProjMatrix.m[0] << std::endl;
		
        // RenderSkybox(view, projection);      // Skybox first (depth trick)
		LevelManager::Update(deltaTime);
		LevelManager::Render();


        // === HUD / Console ===
        glDisable(GL_DEPTH_TEST);
        console.Render(1280, 720);

        SDL_GL_SwapWindow(window);
    }

    // CleanupSkybox();
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}