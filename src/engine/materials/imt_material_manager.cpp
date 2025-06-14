// imt_material_manager.cpp - INC Material Manager

#include "materials/imt_material_manager.h"
#include "materials/imt_texture_loader.h"
#include "shaderapi.h"

#include <iostream>

// This file implements the data-only MaterialManager system,
// which caches raw parsed IMT data (not GPU resources).

Material LoadIMT(const std::string& materialPath); // Forward decl if needed

bool MaterialManager::Load(const std::string& materialPath) {
    if (m_materials.count(materialPath))
        return true;

    Material mat = LoadIMT(materialPath);
    if (mat.shader.empty()) {
        std::cerr << "[MaterialManager] Material has no shader: " << materialPath << "\n";
        return false;
    }

    m_materials[materialPath] = mat;
    return true;
}

const Material* MaterialManager::Get(const std::string& name) const {
    auto it = m_materials.find(name);
    return (it != m_materials.end()) ? &it->second : nullptr;
}
