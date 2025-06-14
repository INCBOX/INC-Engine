// src/filesystem/imdl_loader.cpp
#include "models/imdl_loader.h"
#include <fstream>
#include <sstream>
#include <iostream>

// JSON lib includes
#include <nlohmann/json.hpp>

bool LoadIMDLFromFile(const std::string& filepath, IMDL_CPU& outGeometry) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "[IMDL Loader] Failed to open file: " << filepath << "\n";
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    try {
        // Parse JSON from file contents
        auto jsonData = nlohmann::json::parse(buffer.str());

        // Fill vertexData if present
        if (jsonData.contains("vertices") && jsonData["vertices"].is_array()) {
            outGeometry.vertexData.clear();
            for (const auto& v : jsonData["vertices"])
                outGeometry.vertexData.push_back(v.get<float>());
        }

        // Fill indexData if present
        if (jsonData.contains("indices") && jsonData["indices"].is_array()) {
            outGeometry.indexData.clear();
            for (const auto& i : jsonData["indices"])
                outGeometry.indexData.push_back(i.get<uint32_t>());
        }

        // Fill material string if present
        if (jsonData.contains("material") && jsonData["material"].is_string()) {
            outGeometry.material = jsonData["material"].get<std::string>();
        }

        // Optional: parse other fields here as needed in the future

        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "[IMDL Loader] JSON parse error: " << e.what() << "\n";
        return false;
    }
}