#version 330 core
out vec4 FragColor;
in vec3 WorldPos;

uniform samplerCube environmentMap;


void main()
{
    const float gamma = 2.0f;
    vec3 envColor = texture(environmentMap, WorldPos).rgb;

    // HDR tonemap and gamma correct
    envColor = envColor / (envColor + vec3(1.0f));
    envColor = pow(envColor, vec3(1.0/2.0f));

    FragColor = vec4(envColor, 1.0);
}
