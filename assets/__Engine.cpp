#include <iostream>
#include "Camera/Camera.h"
#include "Levels/TestLevel_1.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define SDL_MAIN_HANDLED // WE ADD THIS TO AVOID MISSING WINMAIN REFERENCE
#include <SDL2/SDL.h>
#include <glad/glad.h>

const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
void main() {
    gl_Position = vec4(aPos, 0.0, 1.0);
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
void main() {
    FragColor = vec4(1.0, 0.0, 0.0, 1.0); // RED
}
)";

int main(int argc, char* argv[]) {
    Camera camera;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(window);

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    LoadTestLevel_1();

    // Compile shaders (for red triangle)
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    float vertices[] = {
        -0.5f, -0.5f,
         0.5f, -0.5f,
         0.0f,  0.5f
    };

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    float aspect = 800.0f / 600.0f;

    while (true) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) break;
        const Uint8* state = SDL_GetKeyboardState(NULL);
        const float cameraSpeed = 2.5f * 0.016f; // Fixed timestep for now (16ms/frame)

        if (state[SDL_SCANCODE_W]) camera.ProcessKeyboard(SDL_SCANCODE_W, cameraSpeed);
        if (state[SDL_SCANCODE_S]) camera.ProcessKeyboard(SDL_SCANCODE_S, cameraSpeed);
        if (state[SDL_SCANCODE_A]) camera.ProcessKeyboard(SDL_SCANCODE_A, cameraSpeed);
        if (state[SDL_SCANCODE_D]) camera.ProcessKeyboard(SDL_SCANCODE_D, cameraSpeed);

        }

        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Render 3D test level
        Mat4 projection = Mat4::perspective(70.0f, aspect, 0.1f, 100.0f);
        RenderTestLevel_1(camera.GetViewMatrix(), projection);
        // Render red triangle (optional)
        //	glUseProgram(shaderProgram); 		A. Remove or comment out this red triangle:
        //	glBindVertexArray(VAO);				A. Remove or comment out this red triangle:	
        //	glDrawArrays(GL_TRIANGLES, 0, 3);	A. Remove or comment out this red triangle:

        SDL_GL_SwapWindow(window);
    }

    UnloadTestLevel_1();
    return 0;
}