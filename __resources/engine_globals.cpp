// engine/engine_globals.h
#pragma once

#include "mathlib/Mat4.h"
#include "Shader.h"

extern Shader gBasicShader;
extern mathlib::Mat4 gProjMatrix;
extern mathlib::Mat4 gViewMatrix;
extern bool gWireframeMode;
