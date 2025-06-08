#include "TestLevel.h"
#include <GL/gl.h>  // Fixed-function OpenGL
#include <cmath>

// Simple rotation angle
static float rotationAngle = 0.0f;

void LoadTestLevel()
{
    // Reset rotation when the level loads
    rotationAngle = 0.0f;

    // Optional: Enable depth testing for correct rendering order
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
}

void RenderTestLevel()
{
    rotationAngle += 0.5f;
    if (rotationAngle >= 360.0f) rotationAngle -= 360.0f;

    // Clear screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset ModelView matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Move back and rotate the cube
    glTranslatef(0.0f, 0.0f, -5.0f);
    glRotatef(rotationAngle, 0.0f, 1.0f, 0.0f);

    glBegin(GL_QUADS);
    // Front Face
    glColor3f(1.0f, 0.0f, 0.0f); 
    glVertex3f(-1.0f, -1.0f,  1.0f); 
    glVertex3f( 1.0f, -1.0f,  1.0f); 
    glVertex3f( 1.0f,  1.0f,  1.0f); 
    glVertex3f(-1.0f,  1.0f,  1.0f); 

    // Back Face
    glColor3f(0.0f, 1.0f, 0.0f); 
    glVertex3f(-1.0f, -1.0f, -1.0f); 
    glVertex3f(-1.0f,  1.0f, -1.0f); 
    glVertex3f( 1.0f,  1.0f, -1.0f); 
    glVertex3f( 1.0f, -1.0f, -1.0f); 

    // Top Face
    glColor3f(0.0f, 0.0f, 1.0f); 
    glVertex3f(-1.0f,  1.0f, -1.0f); 
    glVertex3f(-1.0f,  1.0f,  1.0f); 
    glVertex3f( 1.0f,  1.0f,  1.0f); 
    glVertex3f( 1.0f,  1.0f, -1.0f); 

    // Bottom Face
    glColor3f(1.0f, 1.0f, 0.0f); 
    glVertex3f(-1.0f, -1.0f, -1.0f); 
    glVertex3f( 1.0f, -1.0f, -1.0f); 
    glVertex3f( 1.0f, -1.0f,  1.0f); 
    glVertex3f(-1.0f, -1.0f,  1.0f); 

    // Right Face
    glColor3f(0.0f, 1.0f, 1.0f); 
    glVertex3f( 1.0f, -1.0f, -1.0f); 
    glVertex3f( 1.0f,  1.0f, -1.0f); 
    glVertex3f( 1.0f,  1.0f,  1.0f); 
    glVertex3f( 1.0f, -1.0f,  1.0f); 

    // Left Face
    glColor3f(1.0f, 0.0f, 1.0f); 
    glVertex3f(-1.0f, -1.0f, -1.0f); 
    glVertex3f(-1.0f, -1.0f,  1.0f); 
    glVertex3f(-1.0f,  1.0f,  1.0f); 
    glVertex3f(-1.0f,  1.0f, -1.0f); 
    glEnd();
}

void CleanupTestLevel()
{
    // Nothing to clean in immediate mode
    // Could disable GL states or free future data here
    glDisable(GL_DEPTH_TEST);
}