// camera_spawn.cpp - Source-style camera initialization from info_player_start
#include <vector>
#include <string>
#include "mathlib.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

Vec3 g_CameraOrigin;
Vec3 g_CameraAngles; // pitch, yaw, roll

void ExtractCameraFromMap(const json& data) {
    if (!data.contains("entities")) return;

    for (auto& ent : data["entities"]) {
        std::string classname = ent.value("classname", "");
        if (classname == "info_player_start") {
            auto origin = ent.value("origin", std::vector<float>{0,0,0});
            auto angles = ent.value("angles", std::vector<float>{0,0,0});

            g_CameraOrigin = Vec3(origin[0], origin[1], origin[2]);
            g_CameraAngles = Vec3(angles[0], angles[1], angles[2]);

            std::cout << "[Camera] Spawn set to " << g_CameraOrigin.x << ", "
                      << g_CameraOrigin.y << ", " << g_CameraOrigin.z << " angles: "
                      << g_CameraAngles.x << ", " << g_CameraAngles.y << ", " << g_CameraAngles.z << "\n";
            break;
        }
    }
}

Vec3 GetCameraPosition() {
    return g_CameraOrigin;
}

Vec3 GetCameraForward() {
    float pitch = glm::radians(g_CameraAngles.x);
    float yaw   = glm::radians(g_CameraAngles.y);
    return Vec3(
        cos(pitch) * cos(yaw),
        cos(pitch) * sin(yaw),
        -sin(pitch)
    );
}
