#ifndef TESTLEVEL_H
#define TESTLEVEL_H

#include "Math.h"

void LoadTestLevel();
void RenderTestLevel(const Mat4& view, const Mat4& projection);  // Updated to accept camera matrices
void CleanupTestLevel();

#endif // TESTLEVEL_H
