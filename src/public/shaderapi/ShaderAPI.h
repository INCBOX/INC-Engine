
#pragma once

#include "shaderapi/ishaderapi.h"

extern ShaderAPICore* g_pShaderAPI;

ShaderAPICore* CreateShaderAPI();
void DestroyShaderAPI();
