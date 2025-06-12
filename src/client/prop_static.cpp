// prop_static.cpp - INC PropStatic entity (Source-style prop rendering)
#include "material_manager.h"
#include "mathlib.h"
#include <glad/glad.h>

extern MaterialManager g_MaterialManager;

struct PropStatic {
    Vec3 position;
    std::string materialPath;

    void Draw(const Mat4& view, const Mat4& proj) {
        auto mat = g_MaterialManager.Get(materialPath);
        if (!mat) return;

        Mat4 model = Mat4::Translate(position);

        glUseProgram(mat->shaderID);
        glUniformMatrix4fv(glGetUniformLocation(mat->shaderID, "u_Model"), 1, GL_FALSE, model.data());
        glUniformMatrix4fv(glGetUniformLocation(mat->shaderID, "u_View"), 1, GL_FALSE, view.data());
        glUniformMatrix4fv(glGetUniformLocation(mat->shaderID, "u_Proj"), 1, GL_FALSE, proj.data());

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mat->textureID);
        glUniform1i(glGetUniformLocation(mat->shaderID, "u_Texture"), 0);

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
};
