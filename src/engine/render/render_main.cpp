#include "testrender.h"
#include "camera_spawn.h"
#include "mathlib/matrix.h"
#include <glad/glad.h>

void RenderFrame(int width, int height) {
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Mat4 view = GetViewMatrix();
    Mat4 proj = Mat4::Perspective(75.0f, width / (float)height, 0.1f, 1000.0f);

    R_DrawTestCube(view, proj);
}