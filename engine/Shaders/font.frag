#version 330 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D fontAtlas;

void main() {
    float alpha = texture(fontAtlas, TexCoords).r;
    FragColor = vec4(1.0, 1.0, 1.0, alpha);
}
