#version 330 core
layout (location = 0) in vec3 aPos;
out vec3 TexCoords;

uniform mat4 u_View;
uniform mat4 u_Projection;

void main()
{
    vec4 pos = u_Projection * mat4(mat3(u_View)) * vec4(aPos, 1.0); // remove translation
    gl_Position = pos.xyww; // keep depth at far
    TexCoords = aPos;
}