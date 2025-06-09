
#include "SpaceScene.h"
#include <glad/glad.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <vector>
#include "LevelManagerShader.h"

struct Planet {
    Vec3 position;
    float radius;
    Vec3 color;
};

static std::vector<Planet> planets;
static GLuint vao = 0, vbo = 0;
static GLuint shader = 0;

void InitSpaceScene() {
    std::ifstream file("bin/resources/space_system/planets.json");
    if (!file.is_open()) {
        std::cerr << "[SpaceScene] Failed to open planets.json\n";
        return;
    }

    nlohmann::json data;
    file >> data;

    planets.clear();
    for (const auto& p : data) {
        Planet planet;
        auto pos = p["position"];
        auto col = p["color"];
        planet.position = { pos[0], pos[1], pos[2] };
        planet.radius = p["radius"];
        planet.color = { col[0], col[1], col[2] };
        planets.push_back(planet);
    }

    float verts[] = {
        -1,-1,-1, 1,-1,-1, 1,1,-1, 1,1,-1, -1,1,-1, -1,-1,-1,
        -1,-1, 1, 1,-1, 1, 1,1, 1, 1,1, 1, -1,1, 1, -1,-1, 1,
        -1, 1, 1, -1, 1,-1, -1,-1,-1, -1,-1,-1, -1,-1, 1, -1, 1, 1,
         1, 1, 1, 1, 1,-1, 1,-1,-1, 1,-1,-1, 1,-1, 1, 1, 1, 1,
        -1,-1,-1, 1,-1,-1, 1,-1, 1, 1,-1, 1, -1,-1, 1, -1,-1,-1,
        -1, 1,-1, 1, 1,-1, 1, 1, 1, 1, 1, 1, -1, 1, 1, -1, 1,-1,
    };

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    shader = CreateLevelShader();
}

void RenderSpaceScene(const float* view, const float* projection) {
    glUseProgram(shader);
    glBindVertexArray(vao);

    float vp[16];
    for (int i = 0; i < 16; ++i) vp[i] = projection[i] * view[i];

    GLint vpLoc = glGetUniformLocation(shader, "uViewProj");
    GLint colLoc = glGetUniformLocation(shader, "uColor");

    for (const auto& p : planets) {
        Mat4 model = Mat4::translate(p.position) * Mat4::scale(p.radius);
        Mat4 viewMat = Mat4::fromArray(view);
        Mat4 projMat = Mat4::fromArray(projection);
        Mat4 mvp = projMat * viewMat * model;
        glUniformMatrix4fv(vpLoc, 1, GL_FALSE, mvp.m);
        glUniform3f(colLoc, p.color.x, p.color.y, p.color.z);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}
