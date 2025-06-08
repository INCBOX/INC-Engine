#include "TestLevel.h"
#include <GL/gl.h>
#include <cmath>

static float rotationAngle = 0.0f;

void LoadTestLevel()
{
    rotationAngle = 0.0f;

    glEnable(GL_DEPTH_TEST); // ✅ enable depth testing
	glDepthFunc(GL_LEQUAL);  // ✅ standard depth test function
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f); // bright red

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	
	float fovY = 60.0f;
	float aspect = 1280.0f / 720.0f;
	float nearZ = 0.1f;
	float farZ = 100.0f;
	
	float top = tanf((fovY * 0.5f) * (3.14159f / 180.0f)) * nearZ;
	float bottom = -top;
	float right = top * aspect;
	float left = -right;
	
	glFrustum(left, right, bottom, top, nearZ, farZ);

    glMatrixMode(GL_MODELVIEW);
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

void RenderTestLevel()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // ✅ clear both

    glLoadIdentity();
    glTranslatef(0.0f, -1.5f, -10.0f);
    glRotatef(rotationAngle, 0.0f, 1.0f, 0.0f);

    rotationAngle += 0.3f;

    // Draw floor (large green quad)
    glColor3f(0.2f, 0.8f, 0.3f);
    glBegin(GL_QUADS);
    glVertex3f(-10.0f, 0.0f, -10.0f);
    glVertex3f( 10.0f, 0.0f, -10.0f);
    glVertex3f( 10.0f, 0.0f,  10.0f);
    glVertex3f(-10.0f, 0.0f,  10.0f);
    glEnd();

    // Draw some cubes
    glColor3f(1.0f, 0.0f, 0.0f); DrawCube(-2.0f, 1.0f, 0.0f, 2.0f);
    glColor3f(0.0f, 0.0f, 1.0f); DrawCube(2.0f, 1.0f, 0.0f, 2.0f);
}

void CleanupTestLevel()
{
    // Nothing to clean yet
}