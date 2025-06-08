#version 330 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D fontAtlas;
uniform vec4 uColor;

void main() {
    float alpha = texture(fontAtlas, TexCoords).r;
    FragColor = vec4(uColor.rgb, alpha);
}
