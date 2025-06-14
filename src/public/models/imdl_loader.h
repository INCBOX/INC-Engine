// src/filesystem/imdl_loader.h
#pragma once
#include <string>
#include "models/imdl_cpu.h"

// Loads IMDL model from file path.
// Returns true on success, fills outGeometry.
// Can later switch from JSON to other formats internally without changing this API.
bool LoadIMDLFromFile(const std::string& filepath, IMDL_CPU& outGeometry);
