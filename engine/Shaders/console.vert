#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTex;

out vec2 TexCoords;

uniform mat4 projection;

void main() {
    TexCoords = aTex;
    gl_Position = projection * vec4(aPos.xy, 0.0, 1.0);
}