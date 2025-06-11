#include <iostream>
#include <fstream>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <windows.h>
#include <glad/glad.h>

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

	SDL_Window* window = SDL_CreateWindow("Colorful Triangle", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
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

	std::cout << "[OpenGL Version] " << glGetString(GL_VERSION) << std::endl;
	std::cout << "[Renderer] " << glGetString(GL_RENDERER) << std::endl;

	// Triangle vertex data: position (x, y) + color (r, g, b)
	float vertices[] = {
		// positions        // colors
		-0.5f, -0.5f,       1.0f, 0.0f, 0.0f, // bottom left (red)
		 0.5f, -0.5f,       0.0f, 1.0f, 0.0f, // bottom right (green)
		 0.0f,  0.5f,       0.0f, 0.0f, 1.0f  // top center (blue)
	};

	GLuint vao, vbo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	const char* vertexShaderSrc = R"(#version 330 core
	layout (location = 0) in vec2 aPos;
	layout (location = 1) in vec3 aColor;

	out vec3 vertexColor;

	void main() {
		gl_Position = vec4(aPos, 0.0, 1.0);
		vertexColor = aColor;
	})";

	const char* fragmentShaderSrc = R"(#version 330 core
	in vec3 vertexColor;
	out vec4 FragColor;

	void main() {
		FragColor = vec4(vertexColor, 1.0);
	})";

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSrc, nullptr);
	glCompileShader(vertexShader);

	GLint success;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char log[512];
		glGetShaderInfoLog(vertexShader, 512, nullptr, log);
		LOG_ERROR("Vertex Shader Error:\n" << log);
		return 1;
	}

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSrc, nullptr);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char log[512];
		glGetShaderInfoLog(fragmentShader, 512, nullptr, log);
		LOG_ERROR("Fragment Shader Error:\n" << log);
		return 1;
	}

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		char log[512];
		glGetProgramInfoLog(shaderProgram, 512, nullptr, log);
		LOG_ERROR("Shader Link Error:\n" << log);
		return 1;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// Main loop
	bool running = true;
	SDL_Event event;

	while (running) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || 
				(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
				running = false;
			}
		}

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		SDL_GL_SwapWindow(window);
	}

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteProgram(shaderProgram);

	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}