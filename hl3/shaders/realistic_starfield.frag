#version 330 core
in vec3 TexCoords;
out vec4 FragColor;

uniform samplerCube u_Skybox;

void main()
{
    vec3 color = texture(u_Skybox, TexCoords).rgb;
    FragColor = vec4(color, 1.0);
}
