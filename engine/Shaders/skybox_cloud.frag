#version 330 core

in vec3 TexCoords;
out vec4 FragColor;

uniform samplerCube skybox;
uniform sampler2D cloudLayer;
uniform float u_time;

void main()
{
    vec4 base = texture(skybox, TexCoords);

    // Convert 3D direction to spherical UV
    vec2 uv;
    uv.x = atan(TexCoords.z, TexCoords.x) / (2.0 * 3.14159) + 0.5;
    uv.y = asin(TexCoords.y) / 3.14159 + 0.5;

    // Parallax & distortion
    vec2 uv1 = uv + vec2(u_time * 0.01, 0.0);
    vec2 uv2 = uv * 0.5 + vec2(u_time * 0.003, 0.0);
    uv1 += vec2(sin(uv.y * 30.0 + u_time) * 0.01, cos(uv.x * 20.0 - u_time) * 0.01);

    vec4 c1 = texture(cloudLayer, uv1);
    vec4 c2 = texture(cloudLayer, uv2);

    float blend = mix(c1.r, c2.r, 0.5); // Use red channel as brightness
    float heightFade = smoothstep(0.0, 0.5, TexCoords.y); // Y > 0 = sky only
    float alpha = blend * 0.5 * heightFade;

    FragColor = mix(base, vec4(1.0), alpha);
}
