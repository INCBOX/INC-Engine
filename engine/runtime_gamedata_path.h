#pragma once
#include <string>

// Centralized root for all runtime-loaded files
inline const std::string RUNTIME_DATA_ROOT = "gamedata/";

// Joins root + relative path
inline std::string PATH(const std::string& relative) {
    return RUNTIME_DATA_ROOT + relative;
}

namespace gamedata {

    inline std::string Shader(const std::string& name) {
        return PATH("shaders/" + name);
    }

    inline std::string Texture(const std::string& name) {
        return PATH("textures/" + name);
    }

    inline std::string Script(const std::string& name) {
        return PATH("scripts/" + name);
    }

    inline std::string Model(const std::string& name) {
        return PATH("models/" + name);
    }

    inline std::string Sound(const std::string& name) {
        return PATH("sounds/" + name);
    }

    inline std::string Level(const std::string& name) {
        return PATH("levels/" + name);
    }
}