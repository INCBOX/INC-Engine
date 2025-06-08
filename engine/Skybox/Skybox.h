#pragma once
#include "../Math/Math.h"

void InitSkybox();
void RenderSkybox(const Mat4& view, const Mat4& projection);
void SetSkyboxVisible(bool visible);
void CleanupSkybox();
