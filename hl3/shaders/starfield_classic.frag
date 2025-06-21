#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform float u_Time;

float hash(vec2 p) {
    return fract(sin(dot(p ,vec2(12.9898,78.233))) * 43758.5453);
}

void main() {
    vec2 uv = TexCoords * 10.0;
    float brightness = 0.0;

    vec2 cell = floor(uv);
    vec2 f = fract(uv);

    vec2 starPos = vec2(hash(cell), hash(cell + 1.0));
    float d = length(f - starPos);

    brightness = smoothstep(0.05, 0.0, d) * (0.5 + 0.5 * sin(u_Time * 10.0 + hash(cell) * 6.2831));

    float nebula = smoothstep(0.4, 0.0, length(fract(uv * 0.3) - 0.5)) * 0.3;

    vec3 color = vec3(brightness) + vec3(0.2, 0.1, 0.3) * nebula;

    FragColor = vec4(color, 1.0);
}