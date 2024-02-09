#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec3 aNormal;
layout (location = 1) in vec2 aTexCoords;
layout (location = 3) in mat4 aInstanceMatrix;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float time;
uniform float windStrength;

void main()
{
    // Calculate texture coordinate offset based on time and wind strength
vec2 offset = vec2(0.0, sin(aPos.x * 10.0 + time) * windStrength);

// Apply the offset to texture coordinates
TexCoords = aTexCoords + offset;

gl_Position =projection * view * aInstanceMatrix * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
