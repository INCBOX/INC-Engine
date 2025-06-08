#include "TestLevel.h"
#include <GL/gl.h>  // Or your OpenGL headers
#include <cmath>

// Simple rotation for demo
static float rotationAngle = 0.0f;

void LoadTestLevel()
{
    // Setup level data here, load meshes/textures if any
    rotationAngle = 0.0f;
}

void RenderTestLevel()
{
    rotationAngle += 0.5f;

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

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