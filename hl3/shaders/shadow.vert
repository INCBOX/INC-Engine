#version 330 core
layout(location = 0) in vec3 aPos;

uniform mat4 u_LightMVP;

void main() {
    gl_Position = u_LightMVP * vec4(aPos, 1.0);
}
