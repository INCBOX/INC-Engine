#include "Core/RuntimeDataPath.h"

#include <glad/glad.h>
#include <stb_image.h>
#include <iostream>
#include "skybox_cloud.h"
#include "Skybox.h"

GLuint cloudTexID = 0;
GLuint cloudShader = 0;
float cloudTime = 0.0f;

const char* skyboxCloudVert = R"(#version 330 core

layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoords = aPos;
    vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww; // Keep depth at 1.0 (far)
})";

const char* skyboxCloudFrag = R"(#version 330 core

in vec3 TexCoords;
out vec4 FragColor;

uniform samplerCube skybox;
uniform sampler2D cloudLayer;
uniform float u_time;

void main()
{
    vec4 base = texture(skybox, TexCoords);

    // Convert 3D direction to spherical UV coordinates
    vec2 uv;
    uv.x = atan(TexCoords.z, TexCoords.x) / (2.0 * 3.14159) + 0.5;
    uv.y = asin(TexCoords.y) / 3.14159 + 0.5;

    uv.x += u_time * 0.01; // Scroll horizontally

    vec4 cloud = texture(cloudLayer, uv);
    float alpha = cloud.a * 0.5;

    FragColor = mix(base, cloud, alpha);
})";

void LoadCloudTexture() {
    int w, h, ch;
    stbi_set_flip_vertically_on_load(1);
    unsigned char* data = stbi_load(DataPath("textures/skybox/cloud_layer.png").c_str(), &w, &h, &ch, STBI_rgb_alpha);

    if (!data) {
        std::cerr << "[SkyboxCloud] Failed to load cloud_layer.png\n";
        return;
    }

    glGenTextures(1, &cloudTexID);
    glBindTexture(GL_TEXTURE_2D, cloudTexID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &skyboxCloudVert, nullptr);
    glCompileShader(vs);

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &skyboxCloudFrag, nullptr);
    glCompileShader(fs);

    cloudShader = glCreateProgram();
    glAttachShader(cloudShader, vs);
    glAttachShader(cloudShader, fs);
    glLinkProgram(cloudShader);
    glDeleteShader(vs);
    glDeleteShader(fs);
}

void RenderCloudLayer(const Mat4& view, const Mat4& projection, float deltaTime) {
    cloudTime += deltaTime;

    glUseProgram(cloudShader);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTex);
    glUniform1i(glGetUniformLocation(cloudShader, "skybox"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, cloudTexID);
    glUniform1i(glGetUniformLocation(cloudShader, "cloudLayer"), 1);

    glUniform1f(glGetUniformLocation(cloudShader, "u_time"), cloudTime);
    glUniformMatrix4fv(glGetUniformLocation(cloudShader, "view"), 1, GL_FALSE, view.m);
    glUniformMatrix4fv(glGetUniformLocation(cloudShader, "projection"), 1, GL_FALSE, projection.m);

    glDrawArrays(GL_TRIANGLES, 0, 36); // Uses skybox cube vertices
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}
