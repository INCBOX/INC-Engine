#pragma once

#include "mathlib/Mat4.h"
#include "Shader.h"

// Global shader used across levels
extern Shader gBasicShader;

// Global projection and view matrices
extern mathlib::Mat4 gProjMatrix;
extern mathlib::Mat4 gViewMatrix;

// Wireframe toggle
extern bool gWireframeMode;
