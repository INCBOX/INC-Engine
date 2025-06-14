// render/render_sys.cpp

#include "render/render_sys.h"
#include <iostream>
#include <vector>
#include <memory>

#include <glad/glad.h>
#include <nlohmann/json.hpp>

#include "shaderapi.h"
#include "materials/imt_material_manager.h"

#include "models/imdl_cpu.h"       // CPU-side raw data
#include "models/imdl_gpu.h"       // GPU-side GPU buffers
#include "models/imdl_upload.h"    // UploadIMDLToGPU
#include "models/imdl_loader.h"    // LoadIMDLFromFile
#include "models/imdl_runtime.h"   // Runtime model instance: links CPU + GPU + transform

static std::vector<std::shared_ptr<IMDL_RUNTIME>> g_LoadedModels;  // Runtime model instances
static std::unique_ptr<MaterialManager> g_pMaterialManager;

void Render_Init() {
    std::cout << "[Render] Initialized\n";
    INC::InitShaderAPI();  // Initialize backend (GL, DX, VK)
    g_pMaterialManager = std::make_unique<MaterialManager>();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
}

void Render_LoadMap(const nlohmann::json& mapData) {
    std::cout << "[Render] Loading map data\n";
    g_LoadedModels.clear();

    if (!mapData.contains("entities") || !mapData["entities"].is_array())
        return;

    for (const auto& entity : mapData["entities"]) {
        std::string classname = entity.value("classname", "");
        if (classname == "prop_static") {
            std::string modelPath = entity.value("model", "");
            std::string materialPath = entity.value("material", "");

            std::vector<float> origin = entity.value(
                "origin",
                nlohmann::json::array({0.0f, 0.0f, 0.0f})
            ).get<std::vector<float>>();

            // Load CPU-side geometry
            auto cpuGeom = std::make_shared<IMDL_CPU>();
            if (!LoadIMDLFromFile(modelPath, *cpuGeom)) {
                std::cerr << "[Render] Failed to load model: " << modelPath << "\n";
                continue;
            }

            // Create runtime model instance
            auto runtimeModel = std::make_shared<IMDL_RUNTIME>();
            if (!runtimeModel->InitFromCPU(cpuGeom)) {
                std::cerr << "[Render] Failed to initialize runtime model: " << modelPath << "\n";
                continue;
            }

            runtimeModel->position[0] = origin[0];
            runtimeModel->position[1] = origin[1];
            runtimeModel->position[2] = origin[2];

            // Load material (optionally store on runtime model or material manager)
            if (!materialPath.empty()) {
                g_pMaterialManager->Load(materialPath);
            }

            g_LoadedModels.push_back(runtimeModel);
        }
    }

    std::cout << "[Render] Loaded " << g_LoadedModels.size() << " prop_static entities.\n";
}

void RenderFrame(int width, int height) {
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (const auto& model : g_LoadedModels) {
        model->Render();
    }
}

void Render_Shutdown() {
    std::cout << "[Render] Shutdown\n";
    g_LoadedModels.clear();
    g_pMaterialManager.reset();
    INC::ShutdownShaderAPI();
}
