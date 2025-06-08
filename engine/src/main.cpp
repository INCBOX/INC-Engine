#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <iostream>

const char* vertexShaderSource = R"glsl(
    #version 330 core
    layout (location = 0) in vec3 aPos;

    void main() {
        gl_Position = vec4(aPos, 1.0);
    }
)glsl";

const char* fragmentShaderSource = R"glsl(
    #version 330 core
    out vec4 FragColor;

    void main() {
        FragColor = vec4(0.7, 0.2, 0.1, 1.0); // Martian Red
    }
)glsl";


int main(int argc, char* argv[]) {
    // Initialize SDL with video
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Set OpenGL version (3.3 Core profile)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // Create SDL Window
    SDL_Window* window = SDL_CreateWindow(
        "INC-Engine Test Window",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1280, 720,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
    );

    if (!window) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

	// Create OpenGL Context
	SDL_GLContext glContext = SDL_GL_CreateContext(window);
	if (!glContext) {
		std::cerr << "SDL_GL_CreateContext Error: " << SDL_GetError() << std::endl;
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        SDL_GL_DeleteContext(glContext);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
	
	// ✅ Set viewport size after GLAD is loaded
	glViewport(0, 0, 1280, 720);
	
    // ---- Compile vertex shader ----
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);                   // Create shader object
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);           // Attach GLSL source
    glCompileShader(vertexShader);                                           // Compile it

    // Optional: Check for compile errors
    GLint success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "Vertex Shader compilation failed:\n" << infoLog << std::endl;
    }

    // ---- Compile fragment shader ----
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    // Optional: Check for compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "Fragment Shader compilation failed:\n" << infoLog << std::endl;
    }

    // ---- Link shaders into a shader program ----
    GLuint shaderProgram = glCreateProgram();                      // Create program object
    glAttachShader(shaderProgram, vertexShader);                   // Attach shaders
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);                                  // Link program

    // Optional: Check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "Shader Program linking failed:\n" << infoLog << std::endl;
    }

    // ---- Delete shader objects (they're linked into program now) ----
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);


    // Define a simple triangle (x, y, z)
    float vertices[] = {
         0.0f,  0.5f, 0.0f,  // top
        -0.5f, -0.5f, 0.0f,  // bottom left
         0.5f, -0.5f, 0.0f   // bottom right
    };

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);   // Create VAO (stores the setup)
    glGenBuffers(1, &VBO);        // Create VBO (stores vertex data)

    glBindVertexArray(VAO);      // Bind VAO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);                       // Bind VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);  // Upload data

    // Tell OpenGL how to interpret the vertex data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); 
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0); // unbind VAO


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
	
		glClearColor(0.1f, 0.05f, 0.15f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	
		glUseProgram(shaderProgram);      // Use shader every frame
		glBindVertexArray(VAO);           // Bind VAO
		glDrawArrays(GL_TRIANGLES, 0, 3); // Draw triangle
	
		SDL_GL_SwapWindow(window);
	}


    // Cleanup
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}