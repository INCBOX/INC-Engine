#ifndef HUD_H
#define HUD_H

#include <glad/glad.h>

class HUD {
public:
    HUD();
    ~HUD();
    void Init();
    void RenderQuad(float x, float y, float width, float height, GLuint textureID);
    GLuint GetShaderID() const { return shaderProgram; }

private:
    GLuint quadVAO, quadVBO;
    GLuint shaderProgram;

    GLuint LoadShader(const char* vertexPath, const char* fragmentPath);
    GLuint CompileShader(GLenum type, const char* source);
};

#endif