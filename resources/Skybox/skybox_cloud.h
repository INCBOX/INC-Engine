#pragma once
#include "math.h"

void LoadCloudTexture();
void RenderCloudLayer(const Mat4& view, const Mat4& projection, float deltaTime);
extern unsigned int cloudTexID;
