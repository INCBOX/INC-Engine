#include "engine_Globals.h"	// FIRST! This defines SDL_MAIN_HANDLED

#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <SDL2/SDL_messagebox.h>

#include "CSGOPlayer.h"
#include "Level_TestRoom.h"
#include "engine_Render.h"
#include "mathlib/Mat4.h"

std::ofstream logFile("log.txt");

int main(int argc, char* argv[]) {
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "BOOT", "main() started", nullptr);

	if (!logFile) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Logging Failed", "Could not create log.txt", NULL);
	} else {
		logFile << "[INFO] Logging started." << std::endl;
		logFile << "Logging test line 2" << std::endl;
		logFile.flush();
	}

	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* window = SDL_CreateWindow("Source Test Level",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		1280, 720, SDL_WINDOW_OPENGL);

	SDL_GLContext glContext = SDL_GL_CreateContext(window);
	gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);
	
		if (!GLAD_GL_VERSION_3_3) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "GLAD", "OpenGL 3.3 not supported", nullptr);
		return 1;
	}

	glEnable(GL_DEPTH_TEST);
	SDL_SetRelativeMouseMode(SDL_TRUE);

	// ✅ REQUIRED FOR ANYTHING TO RENDER!
	Render_CreateCubeFloor();

	Level_TestRoom level;
	level.Init();

	CSGOPlayer player;
	player.Init(Vec3(0, 2.0f, -5));  // 2 units above floor, slightly back from center

	Uint64 now = SDL_GetPerformanceCounter();
	Uint64 last = 0;

	const float aspect = 1280.0f / 720.0f;
	const float fovY = 73.74f * (3.1415926f / 180.0f); // radians
	const float nearZ = 0.1f, farZ = 500.0f;
	mathlib::Mat4 proj = mathlib::Mat4::Perspective(fovY, aspect, nearZ, farZ);

	bool running = true;
	SDL_Event event;

	while (running) {
		last = now;
		now = SDL_GetPerformanceCounter();
		float dt = (now - last) / (float)SDL_GetPerformanceFrequency();

		int mouseDX = 0, mouseDY = 0;
		const Uint8* keys = SDL_GetKeyboardState(NULL);

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || keys[SDL_SCANCODE_ESCAPE]) {
				running = false;
			}
			if (event.type == SDL_MOUSEMOTION) {
				mouseDX = event.motion.xrel;
				mouseDY = event.motion.yrel;
			}
		}

		player.HandleInput(keys, mouseDX, mouseDY, dt);

		glClearColor(0.2f, 0.2f, 0.25f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		mathlib::Mat4 viewProj = proj * player.GetViewMatrix();

		std::cout << "[Frame] Rendering level..." << std::endl;
		level.Render(viewProj);

		SDL_GL_SwapWindow(window);
	}

	Render_Cleanup();
	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
