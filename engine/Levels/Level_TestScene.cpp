#include "runtime_gamedata_path.h"
#include "math.h"

#include "Level_TestScene.h"
#include <glad/glad.h>
#include <vector>
#include <fstream>
#include <sstream>

static GLuint vao = 0, vbo = 0;
static std::vector<Vec3> propPositions;

static GLuint shaderProgram = 0;

void Level_TestScene::Init() {
	
	
	
		auto LoadShaderSource = [](const std::string& path) -> std::string {
		std::ifstream file(path);
		std::stringstream buffer;
		buffer << file.rdbuf();
		return buffer.str();
	};
	
	auto CompileShader = [](GLenum type, const std::string& source) -> GLuint {
		GLuint shader = glCreateShader(type);
		const char* src = source.c_str();
		glShaderSource(shader, 1, &src, nullptr);
		glCompileShader(shader);
		return shader;
	};
	
	std::string vertexSrc = LoadShaderSource(gamedata::Shader("basic.vert"));
	std::string fragmentSrc = LoadShaderSource(gamedata::Shader("basic.frag"));
	
	GLuint vs = CompileShader(GL_VERTEX_SHADER, vertexSrc);
	GLuint fs = CompileShader(GL_FRAGMENT_SHADER, fragmentSrc);
	
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vs);
	glAttachShader(shaderProgram, fs);
	glLinkProgram(shaderProgram);
	glDeleteShader(vs);
	glDeleteShader(fs);
	
	
	
    // Simple cube
    float cubeVerts[] = {
        -1,-1,-1,  1,-1,-1,  1, 1,-1,  -1, 1,-1,  // back
        -1,-1, 1,  1,-1, 1,  1, 1, 1,  -1, 1, 1   // front
        // (Add triangles if needed; for brevity, using just positions here)
    };

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerts), cubeVerts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    // Position 100 cubes in a grid
    for (int x = -5; x <= 5; ++x) {
        for (int z = -5; z <= 5; ++z) {
            propPositions.push_back(Vec3(x * 10.0f, 0.0f, z * 10.0f));
        }
    }
}

void Level_TestScene::Update(float /*deltaTime*/) {
    // No animation
}

void Level_TestScene::Render() {
	glUseProgram(shaderProgram);
	
    glBindVertexArray(vao);
    for (const Vec3& pos : propPositions) {
		Mat4 model = Mat4::translate(pos) * Mat4::scale(Vec3(1.0f, 1.0f, 1.0f));
        Mat4 mvp = gProjMatrix * gViewMatrix * model;
		GLint loc = glGetUniformLocation(shaderProgram, "uMVP");
        glUniformMatrix4fv(loc, 1, GL_FALSE, mvp.toGLMatrix());
        glDrawArrays(GL_LINE_LOOP, 0, 8);  // basic debug cube, adjust for triangles
		glUseProgram(0);
    }
    glBindVertexArray(0);
}

void Level_TestScene::Unload() {
    // Clean up if needed
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    propPositions.clear();
}
