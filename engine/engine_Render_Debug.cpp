#include "engine_Render_Debug.h"
#include <iostream>
#include <glad/glad.h>

void PrintMVPMatrix(const mathlib::Mat4& mvp) {
    std::cout << "[MVP Matrix]\n";
    for (int i = 0; i < 4; ++i)
        std::cout << mvp.m[i][0] << " " << mvp.m[i][1] << " " << mvp.m[i][2] << " " << mvp.m[i][3] << "\n";
}

void DebugDrawCall(const mathlib::Mat4& mvp, GLuint uMVP, GLuint shader, int indexCount) {
    std::cout << "[Render] DrawScene called\n";
    std::cout << "[uMVP] location = " << uMVP << "\n";

    PrintMVPMatrix(mvp);

    if (uMVP == -1)
        std::cerr << "[Warning] uMVP uniform not found in shader.\n";

    if (shader == 0)
        std::cerr << "[Warning] Shader program ID is 0 (invalid).\n";

    if (indexCount <= 0)
        std::cerr << "[Warning] Invalid index count for draw call.\n";

    GLint currentProgram = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
    std::cout << "[GL] Currently bound shader = " << currentProgram << "\n";

    GLint vao = 0;
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &vao);
    std::cout << "[GL] Bound VAO = " << vao << "\n";
}
