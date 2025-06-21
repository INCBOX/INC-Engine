#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform float u_Time;

// Simple hash function for pseudo-randomness
float hash(float x) {
    return fract(sin(x) * 43758.5453);
}

// Star color palette for subtle variation
vec3 starColor(float t) {
    if (t < 0.3)
        return mix(vec3(0.8, 0.9, 1.0), vec3(0.5, 0.7, 1.0), t / 0.3);
    else if (t < 0.6)
        return mix(vec3(1.0), vec3(1.0, 0.95, 0.8), (t - 0.3) / 0.3);
    else
        return mix(vec3(1.0, 0.95, 0.8), vec3(1.0, 0.7, 0.4), (t - 0.6) / 0.4);
}

void main() {
    vec2 uv = TexCoords * 2.0 - 1.0; // map to [-1,1]

    vec3 color = vec3(0.0);
    float brightness = 0.0;

    const int NUM_STARS = 100; // fewer stars for performance
    for (int i = 0; i < NUM_STARS; ++i) {
        // Generate star position from hash
        vec2 starPos = vec2(hash(float(i) * 1.23), hash(float(i) * 4.56)) * 2.0 - 1.0;

        // Distance from pixel to star
        float dist = length(uv - starPos);

        // Star size, small but varied a bit
        float size = 0.002 + 0.003 * hash(float(i) * 7.89);

        // Flicker: simple sine based on time and star index
        float flicker = 0.5 + 0.5 * sin(u_Time * 3.0 + float(i) * 12.0);

        // Intensity falls off smoothly with distance
        float intensity = flicker * smoothstep(size, 0.0, dist);

        // Color from palette
        vec3 starCol = starColor(hash(float(i) * 9.87));

        color += starCol * intensity;
        brightness += intensity;
    }

    // Clamp and gamma correct
    brightness = clamp(brightness, 0.0, 1.0);
    color = pow(color * brightness, vec3(1.0 / 2.2));

    FragColor = vec4(color, brightness);
}