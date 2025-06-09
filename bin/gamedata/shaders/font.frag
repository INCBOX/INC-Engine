#version 330 core
in vec2 vTexCoord;
uniform sampler2D uFontAtlas;
uniform vec3 uTextColor;
out vec4 FragColor;

void main() {
    vec4 sampled = texture(uFontAtlas, vTexCoord);
    if (sampled.a < 0.1)
        discard;
    FragColor = vec4(uTextColor, sampled.a);
}
