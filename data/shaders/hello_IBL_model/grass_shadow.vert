#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec3 aNormal;
layout (location = 1) in vec2 aTexCoords;
layout (location = 3) in mat4 aInstanceMatrix;

out vec2 TexCoords;
out vec4 FragPosLightSpace;
out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float time;
uniform float windStrength;

uniform mat4 lightSpaceMatrix;

void main()
{
    // Calculate texture coordinate offset based on time and wind strength
vec2 offset = vec2(0.0, sin(aPos.x * 10.0 + time) * windStrength);

// Apply the offset to texture coordinates
TexCoords = aTexCoords + offset;
Normal = transpose(inverse(mat3(model))) * aNormal;
FragPos = vec3(model * vec4(aPos, 1.0));
FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);

gl_Position =projection * view * aInstanceMatrix * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
