#pragma once
#include "mathlib/mat4.h"
#include <glad/glad.h>  // Needed for GLuint

void CheckShaderCompile(GLuint shader, const char* label);
void CheckProgramLink(GLuint program, const char* label);
void PrintMVPMatrix(const mathlib::Mat4& mvp);

void DebugDrawCall(const mathlib::Mat4& mvp, GLuint uMVP, GLuint program, int indexCount);
