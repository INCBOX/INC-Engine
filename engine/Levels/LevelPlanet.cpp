#include "runtime_gamedata_path.h"

#include "LevelPlanet.h"
#include <glad/glad.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

static GLuint vao = 0, vbo = 0;
static GLuint sphereVAO = 0, sphereVBO = 0;
static GLuint planetShader = 0;
static std::vector<float> sphereVertices;

GLuint LoadPlanetShader(const char* vertPath, const char* fragPath) {
    auto load = [](const char* path) -> std::string {
        std::ifstream file(path);
        std::stringstream s;
        s << file.rdbuf();
        return s.str();
    };

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    std::string vsrc = load(vertPath);
    const char* vcstr = vsrc.c_str();
    glShaderSource(vs, 1, &vcstr, nullptr);
    glCompileShader(vs);

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    std::string fsrc = load(fragPath);
    const char* fcstr = fsrc.c_str();
    glShaderSource(fs, 1, &fcstr, nullptr);
    glCompileShader(fs);

    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);
    return program;
}

// === Generate Sphere Vertex Data (no indices, unlit for now) ===
void GenerateSphereMesh(int sectorCount = 36, int stackCount = 18) {
    const float PI = 3.1415926535f;
    float x, y, z, xy;

    for (int i = 0; i <= stackCount; ++i) {
        float stackAngle = PI / 2 - i * (PI / stackCount);
        xy = cosf(stackAngle);
        z = sinf(stackAngle);

        for (int j = 0; j <= sectorCount; ++j) {
            float sectorAngle = j * (2 * PI / sectorCount);
            x = xy * cosf(sectorAngle);
            y = xy * sinf(sectorAngle);
            sphereVertices.push_back(x);
            sphereVertices.push_back(y);
            sphereVertices.push_back(z);
        }
    }
}

LevelPlanet::LevelPlanet(const std::string& name, const Vec3& pos, float radius, const Vec3& color)
    : name(name), position(pos), radius(radius), color(color) {}

void LevelPlanet::Init() {
    if (initialized) return;
    initialized = true;
    std::cout << "[LevelPlanet] Init: " << name << "\n";

	planetShader = LoadPlanetShader(
		gamedata::Shader("planet.vert").c_str(),
		gamedata::Shader("planet.frag").c_str()
	);


    // === OPTION 1: Render as CUBE ===
    float s = radius;
    float cubeVerts[] = {
        -s,-s, s,  s,-s, s,  s, s, s,   s, s, s,  -s, s, s,  -s,-s, s,
        -s,-s,-s, -s, s,-s,  s, s,-s,   s, s,-s,  s,-s,-s,  -s,-s,-s,
        -s,-s,-s, -s,-s, s, -s, s, s,  -s, s, s, -s, s,-s, -s,-s,-s,
         s,-s,-s,  s, s,-s,  s, s, s,   s, s, s,  s,-s, s,  s,-s,-s,
        -s, s,-s, -s, s, s,  s, s, s,   s, s, s,  s, s,-s, -s, s,-s,
        -s,-s,-s,  s,-s,-s,  s,-s, s,   s,-s, s, -s,-s, s, -s,-s,-s
    };

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerts), cubeVerts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    if (sphereVertices.empty()) {
        GenerateSphereMesh();
    }
    glGenVertexArrays(1, &sphereVAO);
    glGenBuffers(1, &sphereVBO);
    glBindVertexArray(sphereVAO);
    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(float), &sphereVertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}

void LevelPlanet::Update(float) {}

void LevelPlanet::Render(const float* view, const float* projection) {
    if (!initialized) return;

    Mat4 model = Mat4::translate(position) * Mat4::scale(radius);
    Mat4 mvp = Mat4::fromArray(projection) * Mat4::fromArray(view) * model;

    glUseProgram(planetShader);

    GLint loc = glGetUniformLocation(planetShader, "u_mvp");
    glUniformMatrix4fv(loc, 1, GL_FALSE, mvp.m);

    GLint colorLoc = glGetUniformLocation(planetShader, "u_color");
    glUniform3f(colorLoc, color.x, color.y, color.z);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    // To use sphere:
    // glBindVertexArray(sphereVAO);
    // glDrawArrays(GL_POINTS, 0, sphereVertices.size() / 3);
}

void LevelPlanet::Unload() {
    std::cout << "[LevelPlanet] Unload: " << name << "\n";
}

const char* LevelPlanet::GetName() const {
    return name.c_str();
}

const Vec3& LevelPlanet::GetPosition() const {
    return position;
}