// imt_material_loader.cpp - loads .imt (INC Material Format v1.0)
// Pure file loader. No engine or rendering dependencies here.

#include <string>
#include <fstream>
#include <unordered_map>
#include <iostream>
#include <filesystem>
#include "imt_material.h"

// Declare FS_ResolvePath function pointer from filesystem_stdio.dll
using FS_ResolvePathFn = std::string(*)(const std::string&);
extern FS_ResolvePathFn FS_ResolvePath;

Material LoadIMT(const std::string& relativePath) {
    Material mat;

    if (!FS_ResolvePath) {
        std::cerr << "[MaterialLoader] FS_ResolvePath is null!\n";
        return mat;
    }

    std::string fullPath = FS_ResolvePath("materials/" + relativePath + ".imt");

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

        if (quote4 == std::string::npos) {
            std::cerr << "[MaterialLoader] Malformed line: " << line << "\n";
            continue;
        }

        std::string key = line.substr(quote1 + 1, quote2 - quote1 - 1);
        std::string val = line.substr(quote3 + 1, quote4 - quote3 - 1);

        if (!readHeader && key == "INC_Material") {
            mat.version = val;
            readHeader = true;
            continue;
        }

        if (key == "shader") {
            mat.shader = val;
        } else {
            // Known standard Source-style material keys
            if (key == "$basetexture") {
                mat.parameters[key] = val;
            } else if (key == "$bumpmap") {
                mat.parameters[key] = val;
                std::cout << "[MaterialLoader] Recognized future bumpmap: " << val << "\n";
            } else if (key == "$roughness" || key == "$metallic") {
                mat.parameters[key] = val;
                std::cout << "[MaterialLoader] Recognized future PBR key: " << key << " = " << val << "\n";
            } else if (key == "$translucent" || key == "$alpha") {
                mat.parameters[key] = val;
            } else if (key == "$surfaceprop") {
                mat.parameters[key] = val;
            } else {
                std::cout << "[MaterialLoader] Unrecognized material key: " << key << " = " << val << "\n";
                mat.parameters[key] = val;
            }
        }
    }

    std::cout << "[MaterialLoader] IMT v" << mat.version << ", shader: " << mat.shader << "\n";
    for (const auto& [key, value] : mat.parameters) {
        std::cout << "  " << key << " = " << value << "\n";
    }

    return mat;
}