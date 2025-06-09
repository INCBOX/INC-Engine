#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D uFontAtlas;
uniform vec3 uTextColor;

void main() {
    float alpha = texture(uFontAtlas, TexCoord).r;  // Sample red channel from GL_RED
    if (alpha < 0.1)
        discard; // Optional: makes characters clean by skipping transparent pixels
    FragColor = vec4(uTextColor, alpha);
}
