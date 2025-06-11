
#include "Level_TestRoom.h"
#include "engine_render.h"
#include "engine_shaders.h"
#include "runtime_GameData_PATH.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

using json = nlohmann::json;
using namespace mathlib;

struct Box {
    Vec3 pos;
    Vec3 size;
    Vec3 color;
};

static std::vector<Box> gLevelGeometry;
static Vec3 gPlayerSpawnPos;

void LoadLevelGeometry(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "[Level] Failed to load: " << path << std::endl;
        return;
    }

    json levelData;
    file >> levelData;

    gLevelGeometry.clear();
    for (auto& obj : levelData["objects"]) {
        if (obj["type"] == "box") {
            Box b;
            b.pos = Vec3(obj["position"][0], obj["position"][1], obj["position"][2]);
            b.size = Vec3(obj["size"][0], obj["size"][1], obj["size"][2]);
            b.color = obj.contains("color") ?
                      Vec3(obj["color"][0], obj["color"][1], obj["color"][2]) :
                      Vec3(1.0f, 0.5f, 0.0f); // default orange
            gLevelGeometry.push_back(b);
        }
    }

    auto spawn = levelData["player_spawn"];
    gPlayerSpawnPos = Vec3(spawn["position"][0], spawn["position"][1], spawn["position"][2]);
}

void Level_TestRoom::Init() {
    LoadLevelGeometry(gamedata::Level("testroom.json"));
    Render_CreateCubeFloor(); // temp: shared mesh
}

void Level_TestRoom::Update(float dt) {
    // update player, physics, etc (not yet)
}

void Level_TestRoom::Render(const Mat4& viewProj) {
    std::cout << "[Level] Render called\n";

    for (const auto& box : gLevelGeometry) {
        Mat4 model = Mat4::Translate(box.pos) * Mat4::Scale(box.size);
        Mat4 mvp = viewProj * model;
        Render_DrawScene(mvp);
    }
}


Vec3 Level_TestRoom::GetPlayerSpawn() const {
    return gPlayerSpawnPos;
}