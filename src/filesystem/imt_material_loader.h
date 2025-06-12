#pragma once
#include <string>
#include <unordered_map>

// This structure stores the parsed result from .imt files.
// Used only by the filesystem-level material loader. No rendering logic.
struct Material {
    std::string version;
    std::string shader;
    std::unordered_map<std::string, std::string> parameters;
};

// Loads a material from materials/<relativePath>.imt
// Example: "brick/brickwall" -> materials/brick/brickwall.imt
// File format v1.0 is simple text-based, similar to Valve's VMTs.
// Future v2 may use JSON if needed. Do not implement yet — just noted.
//
// Recognized keys:
// - shader
// - $basetexture
// - $bumpmap       (not used yet — reserved for future normal maps)
// - $roughness     (reserved for future PBR)
// - $metallic      (reserved for future PBR)
// - $translucent
// - $alpha
// - $surfaceprop
//
// This is a pure data loader — no engine dependencies like glad or SDL.
//
Material LoadIMT(const std::string& relativePath);
