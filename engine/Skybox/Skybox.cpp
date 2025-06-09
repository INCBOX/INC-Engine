#include "Core/RuntimeDataPath.h"
#include "Skybox.h"
#include <glad/glad.h>
#include <vector>
#include <string>
#include <iostream>
#include <stb_image.h>
#include "../Math/Math.h"
#include "skybox_cloud.h"


static GLuint skyboxVAO = 0, skyboxVBO = 0;
unsigned int cubemapTex = 0;
static GLuint shader = 0;
static bool skyboxVisible = true;

const char* skyboxVert = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main() {
    TexCoords = aPos;
    vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}
)";

const char* skyboxFrag = R"(
#version 330 core
in vec3 TexCoords;
out vec4 FragColor;

uniform samplerCube skybox;

void main() {
    FragColor = texture(skybox, TexCoords);
}
)";

static float cubeVertices[] = {
    -1.0f,  1.0f, -1.0f,  -1.0f, -1.0f, -1.0f,   1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,   1.0f,  1.0f, -1.0f,  -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,  -1.0f, -1.0f, -1.0f,  -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,  -1.0f,  1.0f,  1.0f,  -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,   1.0f, -1.0f,  1.0f,   1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,   1.0f,  1.0f, -1.0f,   1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,  -1.0f,  1.0f,  1.0f,   1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,   1.0f, -1.0f,  1.0f,  -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,   1.0f,  1.0f, -1.0f,   1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,  -1.0f,  1.0f,  1.0f,  -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,  -1.0f, -1.0f,  1.0f,   1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,  -1.0f, -1.0f,  1.0f,   1.0f, -1.0f,  1.0f
};

static GLuint compileShader(GLenum type, const char* src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);
    GLint ok;
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[512];
        glGetShaderInfoLog(s, 512, nullptr, log);
        std::cerr << "Shader compile error: " << log << std::endl;
    }
    return s;
}

GLuint loadCubemap(const std::vector<std::string>& faces) {
    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texID);

    for (GLuint i = 0; i < faces.size(); i++) {
        std::cout << "[Skybox] Loading face: " << faces[i] << std::endl;
        int w, h, ch;
        unsigned char* data = stbi_load(faces[i].c_str(), &w, &h, &ch, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
            std::cerr << "[Skybox] Failed to load cubemap face: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return texID;
}

void InitSkybox() {
	LoadCloudTexture();
	
    std::vector<std::string> faces = {
        DataPath("textures/skybox/jettelly_space_common_black_RIGHT.png"),
        DataPath("textures/skybox/jettelly_space_common_black_LEFT.png"),
        DataPath("textures/skybox/jettelly_space_common_black_UP.png"),
        DataPath("textures/skybox/jettelly_space_common_black_DOWN.png"),
        DataPath("textures/skybox/jettelly_space_common_black_FRONT.png"),
        DataPath("textures/skybox/jettelly_space_common_black_BACK.png")
    };

    cubemapTex = loadCubemap(faces);

    GLuint vs = compileShader(GL_VERTEX_SHADER, skyboxVert);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, skyboxFrag);
    shader = glCreateProgram();
    glAttachShader(shader, vs);
    glAttachShader(shader, fs);
    glLinkProgram(shader);
    glDeleteShader(vs);
    glDeleteShader(fs);

    glUseProgram(shader);
    glUniform1i(glGetUniformLocation(shader, "skybox"), 0); // 🧠 Link sampler

    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}

void SetSkyboxVisible(bool visible) {
    skyboxVisible = visible;
}

void RenderSkybox(const Mat4& view, const Mat4& projection) {
    if (!skyboxVisible) return;

    // Zero out translation in view matrix
    Mat4 viewNoTrans = view;
    viewNoTrans.m[12] = 0.0f;
    viewNoTrans.m[13] = 0.0f;
    viewNoTrans.m[14] = 0.0f;

    glDepthFunc(GL_LEQUAL);
    glUseProgram(shader);

    GLint v = glGetUniformLocation(shader, "view");
    GLint p = glGetUniformLocation(shader, "projection");
    glUniformMatrix4fv(v, 1, GL_FALSE, viewNoTrans.m);
    glUniformMatrix4fv(p, 1, GL_FALSE, projection.m);

    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTex);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
	
	RenderCloudLayer(view, projection, 0.016f); // Use real deltaTime if passed in
}

void CleanupSkybox() {
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);
    glDeleteProgram(shader);
    glDeleteTextures(1, &cubemapTex);
}