// material_loader.cpp - loads .imt (INC Material Format v1.0)
#include <string>
#include <fstream>
#include <unordered_map>
#include <iostream>
#include <filesystem>
#include "filesystem_stdio.h"

struct Material {
    std::string version;
    std::string shader;
    std::unordered_map<std::string, std::string> parameters;
};

Material LoadIMT(const std::string& relativePath) {
    Material mat;
    std::string fullPath = FS_ResolvePath("materials/" + relativePath);

    std::ifstream file(fullPath);
    if (!file.is_open()) {
        std::cerr << "[MaterialLoader] Failed to open: " << fullPath << "\n";
        return mat;
    }

    std::string line;
    bool readHeader = false;

    while (std::getline(file, line)) {
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        if (line.empty() || line[0] == '/' || line[0] == '{' || line[0] == '}')
            continue;

        auto quote1 = line.find('"');
        auto quote2 = line.find('"', quote1 + 1);
        auto quote3 = line.find('"', quote2 + 1);
        auto quote4 = line.find('"', quote3 + 1);

        if (!readHeader && quote1 != std::string::npos && quote2 != std::string::npos && quote3 != std::string::npos && quote4 != std::string::npos) {
            std::string id = line.substr(quote1 + 1, quote2 - quote1 - 1);
            std::string ver = line.substr(quote3 + 1, quote4 - quote3 - 1);
            if (id == "INC_Material") {
                mat.version = ver;
                readHeader = true;
                continue;
            }
        }

        if (quote1 != std::string::npos && quote2 != std::string::npos &&
            quote3 != std::string::npos && quote4 != std::string::npos) {
            std::string key = line.substr(quote1 + 1, quote2 - quote1 - 1);
            std::string val = line.substr(quote3 + 1, quote4 - quote3 - 1);
            if (key == "shader") mat.shader = val;
            else mat.parameters[key] = val;
        }
    }

    std::cout << "[MaterialLoader] IMT v" << mat.version << ", shader: " << mat.shader << "\n";
    for (const auto& [key, value] : mat.parameters) {
        std::cout << "  " << key << " = " << value << "\n";
    }

    return mat;
}
