#include "renderer/gl_starfield_renderer.h"
#include <glad/glad.h>
#include <iostream>

// STARFIELD
bool GLStarfieldRenderer::LoadStarfieldShaders() {
    m_StarfieldShader = std::make_unique<ShaderProgram>();
    if (!m_StarfieldShader->CompileFromFile("hl3/shaders/starfield.vert", "hl3/shaders/starfield.frag")) {
        std::cerr << "[GL] Starfield shader compilation failed\n";
        return false;
    }

    InitStarfieldGeometry();
    return true;
}

void GLStarfieldRenderer::InitStarfieldGeometry() {
    // Fullscreen quad (2 triangles)
    float vertices[] = {
        // positions   // texCoords
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f,

        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f,
        -1.0f,  1.0f,  0.0f, 1.0f
    };

    glGenVertexArrays(1, &m_StarfieldVAO);
    glGenBuffers(1, &m_StarfieldVBO);

    glBindVertexArray(m_StarfieldVAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_StarfieldVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); // position
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1); // texCoords
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindVertexArray(0);
}
// STARFIELD
void GLStarfieldRenderer::RenderStarfield(float elapsedTime) {
    m_StarfieldShader->Use();

    int timeLocation = glGetUniformLocation(m_StarfieldShader->ID, "u_Time");
    glUniform1f(timeLocation, elapsedTime);

    glBindVertexArray(m_StarfieldVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
	glUseProgram(0);
}
// STARFIELD
void GLStarfieldRenderer::ReleaseStarfield() {
    if (m_StarfieldShader) {
        m_StarfieldShader->Delete();
        m_StarfieldShader.reset();
    }

    CleanupStarfieldGeometry();
}
// STARFIELD
void GLStarfieldRenderer::CleanupStarfieldGeometry() {
    if (m_StarfieldVBO) {
        glDeleteBuffers(1, &m_StarfieldVBO);
        m_StarfieldVBO = 0;
    }
    if (m_StarfieldVAO) {
        glDeleteVertexArrays(1, &m_StarfieldVAO);
        m_StarfieldVAO = 0;
    }
}
// STARFIELD
void GLStarfieldRenderer::SetDepthTestEnabled(bool enabled) {
    if (enabled) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }
}
// STARFIELD
void GLStarfieldRenderer::SetDepthMaskEnabled(bool enabled) {
    glDepthMask(enabled ? GL_TRUE : GL_FALSE);
}