// world_loader.cpp - Parses start.json and populates the world
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include "filesystem_stdio.h"
#include "world_render.h"
#include "prop_static.h"
#include "material_manager.h"

using json = nlohmann::json;
extern MaterialManager g_MaterialManager;

bool World_LoadFromJSON(const std::string& mapName) {
    std::string rel = "maps/" + mapName + ".json";
    std::string full = FS_ResolvePath(rel);
    if (full.empty()) {
        std::cerr << "[WorldLoader] Map not found: " << rel << "\n";
        return false;
    }

    std::ifstream file(full);
    if (!file.is_open()) {
        std::cerr << "[WorldLoader] Failed to open map file: " << full << "\n";
        return false;
    }

    json data;
    try {
        file >> data;
    } catch (const std::exception& e) {
        std::cerr << "[WorldLoader] JSON parse error: " << e.what() << "\n";
        return false;
    }

    if (!data.contains("entities")) return true;

    for (auto& ent : data["entities"]) {
        std::string classname = ent.value("classname", "");
        if (classname == "prop_static") {
			auto model = ent.value("model", "models/testcube.imdl");
			LoadIMDL(model, prop.model);

            PropStatic prop;
            prop.position = Vec3(origin[0], origin[1], origin[2]);
            prop.materialPath = mat;
            g_MaterialManager.Load(mat);
            World_AddStaticProp(prop);
        }
    }

    std::cout << "[WorldLoader] Map loaded: " << mapName << "\n";
    return true;
}
