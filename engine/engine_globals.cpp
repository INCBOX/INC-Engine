#pragma once
#include "mathlib/Mat4.h"
class Shader;

extern bool gWireframeMode;
float gCameraSpeed = 5.0f;
extern Shader gBasicShader;
extern mathlib::Mat4 gViewMatrix;
extern mathlib::Mat4 gProjMatrix;