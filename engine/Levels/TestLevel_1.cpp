#include "TestLevel_1.h"
#include "../Math/Math.h"
#include <GL/gl.h>
#include <cmath>

static float rotationAngle = 0.0f;

void LoadTestLevel_1()
{
    rotationAngle = 0.0f;

    glEnable(GL_DEPTH_TEST);        // Enable depth testing
    glDepthFunc(GL_LEQUAL);
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f); // Bright red background
}

void DrawCube(float x, float y, float z, float size)
{
    float hs = size / 2.0f;

    glPushMatrix();
    glTranslatef(x, y, z);

    glBegin(GL_QUADS);
    // Front
    glVertex3f(-hs, -hs,  hs); glVertex3f(hs, -hs,  hs);
    glVertex3f(hs,  hs,  hs); glVertex3f(-hs,  hs,  hs);
    // Back
    glVertex3f(-hs, -hs, -hs); glVertex3f(-hs,  hs, -hs);
    glVertex3f(hs,  hs, -hs); glVertex3f(hs, -hs, -hs);
    // Top
    glVertex3f(-hs,  hs, -hs); glVertex3f(-hs,  hs,  hs);
    glVertex3f(hs,  hs,  hs); glVertex3f(hs,  hs, -hs);
    // Bottom
    glVertex3f(-hs, -hs, -hs); glVertex3f(hs, -hs, -hs);
    glVertex3f(hs, -hs,  hs); glVertex3f(-hs, -hs,  hs);
    // Right
    glVertex3f(hs, -hs, -hs); glVertex3f(hs,  hs, -hs);
    glVertex3f(hs,  hs,  hs); glVertex3f(hs, -hs,  hs);
    // Left
    glVertex3f(-hs, -hs, -hs); glVertex3f(-hs, -hs,  hs);
    glVertex3f(-hs,  hs,  hs); glVertex3f(-hs,  hs, -hs);
    glEnd();

    glPopMatrix();
}

void RenderTestLevel_1(const Mat4& view, const Mat4& projection)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(projection.toGLMatrix()); // ✅ Use your math library's matrix

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(view.toGLMatrix());

    glPushMatrix();

    // Draw floor
    glColor3f(0.2f, 0.8f, 0.3f);
    glBegin(GL_QUADS);
    glVertex3f(-10.0f, 0.0f, -10.0f);
    glVertex3f( 10.0f, 0.0f, -10.0f);
    glVertex3f( 10.0f, 0.0f,  10.0f);
    glVertex3f(-10.0f, 0.0f,  10.0f);
    glEnd();

    // Draw two cubes
    glColor3f(1.0f, 0.0f, 0.0f); DrawCube(-2.0f, 1.0f, 0.0f, 2.0f);
    glColor3f(0.0f, 0.0f, 1.0f); DrawCube( 2.0f, 1.0f, 0.0f, 2.0f);

    glPopMatrix();
}

void UnloadTestLevel_1()
{
    // Nothing to clean yet
}