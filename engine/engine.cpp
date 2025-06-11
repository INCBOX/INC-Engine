#include <iostream>
#include <chrono>
#include <fstream>


#define SDL_MAIN_HANDLED // WE ADD THIS TO AVOID MISSING WINMAIN REFERENCE
#include <SDL2/SDL.h> 
#include <windows.h>
#include <filesystem>	// must come after
#include <glad/glad.h>

#include "runtime_gamedata_path.h"
#include "engine_globals.h"

#include "mathlib/Mat4.h"
#include "console.h"
#include "hud.h"
#include "Shader.h"
#include "FPS_controller.h"
#include "Levels/LevelManager.h"
#include "Levels/Level_TestScene.h"

using namespace mathlib;

Shader gBasicShader;
mathlib::Mat4 gProjMatrix;
mathlib::Mat4 gViewMatrix;
bool gWireframeMode = false;

int main(int argc, char* argv[]) {
	// Redirect error output to a log file
    std::ofstream errorLog("engine_error.log", std::ios::out);
    #define LOG_ERROR(msg) do { std::cerr << msg << std::endl; errorLog << msg << std::endl; } while(0)

	std::cout << "Wireframe: " << (gWireframeMode ? "ON" : "OFF") << std::endl;
	std::cout << "[RenderMode] " << (gWireframeMode ? "GL_LINE" : "GL_FILL") << std::endl;
	std::cerr << "[OpenGL] Shader compilation succeeded\n";
	std::cerr << "[OpenGL] Program linking succeeded\n";
	
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
	
	// --- Load and compile shader ---
	using namespace gamedata;
	if (!gBasicShader.Load(gamedata::Shader("basic.vert"), gamedata::Shader("basic.frag"))) {
        LOG_ERROR("Failed to load basic shader");
        return 1;
    }

    SDL_SetRelativeMouseMode(SDL_TRUE);
    glViewport(0, 0, 1280, 720);
    glEnable(GL_DEPTH_TEST);

    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    float aspectRatio = (float)w / (float)h;

	FPSController player(Vec3(0.0f, 2.0f, 5.0f));
    HUD hud;
    hud.Init();
    Console console(&hud);

    LevelManager::SetLevel(std::make_unique<Level_TestScene>());

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
				continue; // Skip processing the backtick
			}
			
			console.ProcessEvent(event);

            if (!console.IsActive() && event.type == SDL_MOUSEMOTION) {
                player.HandleMouseMotion((float)event.motion.xrel, (float)-event.motion.yrel);
            }
        }
		
        if (!console.IsActive()) {
            const Uint8* keys = SDL_GetKeyboardState(nullptr);
            player.Update(deltaTime, keys);
        }
		
		glClearColor(0.15f, 0.02f, 0.02f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glPolygonMode(GL_FRONT_AND_BACK, gWireframeMode ? GL_LINE : GL_FILL);
        glEnable(GL_DEPTH_TEST);
		
        gViewMatrix = player.GetViewMatrix();
        gProjMatrix = player.GetProjectionMatrix(aspectRatio);

        // Bind shader and set common uniforms
        gBasicShader.Bind();
		gBasicShader.SetUniformMat4("uView", gViewMatrix.Data());
		gBasicShader.SetUniformMat4("uProjection", gProjMatrix.Data());
		
        Mat4 modelMatrix = Mat4::Identity();
        gBasicShader.SetUniformMat4("uModel", modelMatrix.Data());

        // Debug output (optional)
        Vec3 camPos = player.GetPosition();
        std::cout << "[Camera Pos] " << camPos.x << ", " << camPos.y << ", " << camPos.z << std::endl;
		
		// === Scene ===
		LevelManager::Update(deltaTime);
		LevelManager::Render();
		
        gBasicShader.Unbind();

        // === HUD / Console ===
        glDisable(GL_DEPTH_TEST);
        console.Render(w, h);

        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}