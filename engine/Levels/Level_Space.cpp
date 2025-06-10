#include "Level_Space.h"
#include "../runtime_gamedata_path.h"
#include "../math.h"
#include <nlohmann/json.hpp>
#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <cmath>

using json = nlohmann::json;

extern Mat4 gViewMatrix;
extern Mat4 gProjMatrix;

static std::string LoadTextFile(const std::string& path) {
    std::ifstream file(path);
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

static unsigned int CompileShader(unsigned int type, const std::string& source) {
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (!result) {
        char msg[1024];
        glGetShaderInfoLog(id, 1024, nullptr, msg);
        std::cerr << "[Shader Error] " << msg << std::endl;
        return 0;
    }
    return id;
}

static unsigned int CreateShaderProgram(const std::string& vertexPath, const std::string& fragmentPath) {
    std::string vertexCode = LoadTextFile(vertexPath);
    std::string fragmentCode = LoadTextFile(fragmentPath);

    unsigned int vertexShader = CompileShader(GL_VERTEX_SHADER, vertexCode);
    unsigned int fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentCode);
    unsigned int program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

std::vector<Planet> LoadPlanets(const std::string& file) {
    std::ifstream in(file);
    if (!in.is_open()) {
        std::cerr << "[ERROR] Failed to open planet file: " << file << std::endl;
        return {};
    }

    json j;
    in >> j;

    std::vector<Planet> result;
    for (const auto& item : j) {
        Planet p;
        p.name = item["name"];
        p.position = Vec3(item["position"][0], item["position"][1], item["position"][2]);
        p.radius = item["radius"];
        p.texturePath = item["texture"];
        result.push_back(p);
    }
    return result;
}

void SphereMesh::Init() {
    const int sectorCount = 18;
    const int stackCount = 18;
    const float PI = 3.14159265359f;

    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    for (int i = 0; i <= stackCount; ++i) {
        float stackAngle = PI / 2 - i * PI / stackCount;
        float xy = cosf(stackAngle);
        float z = sinf(stackAngle);

        for (int j = 0; j <= sectorCount; ++j) {
            float sectorAngle = j * 2 * PI / sectorCount;
            float x = xy * cosf(sectorAngle);
            float y = xy * sinf(sectorAngle);
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
        }
    }

    for (int i = 0; i < stackCount; ++i) {
        int k1 = i * (sectorCount + 1);
        int k2 = k1 + sectorCount + 1;

        for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
            if (i != 0) {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }
            if (i != (stackCount - 1)) {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }

    indexCount = indices.size();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void SphereMesh::Render() {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void SphereMesh::Cleanup() {
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
}

void Level_Space::Init() {
    std::cout << "Initializing space level...\n";
    planets = LoadPlanets(gamedata::Level("planets.json"));
    shaderID = CreateShaderProgram(gamedata::Shader("planet.vert"), gamedata::Shader("planet.frag"));
    sphereMesh.Init();
}

void Level_Space::Update(float deltaTime) {}

void Level_Space::Render() {
    glUseProgram(shaderID);
    for (const auto& planet : planets) {
        float scale = planet.radius * 0.005f;
        Mat4 model = Mat4::translate(planet.position) * Mat4::scale(scale);
        Mat4 mvp = gProjMatrix * gViewMatrix * model;
        glUniformMatrix4fv(glGetUniformLocation(shaderID, "u_MVP"), 1, GL_FALSE, mvp.toGLMatrix());
        sphereMesh.Render();
    }
    glUseProgram(0);
}

void Level_Space::Unload() {
    sphereMesh.Cleanup();
}