#version 330 core

layout(location = 0) in vec2 aPos;       // position (clip space quad)
layout(location = 1) in vec2 aTexCoords; // texcoords

out vec2 TexCoords;

// uniform mat4 u_MVP;  // THIS IS NEED FOR STAR CITIZEN STYLE STARFIELD

void main()
{
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 0.0, 1.0);
}
