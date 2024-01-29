#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float time;
uniform float windStrength;

void main()
{
    TexCoords = aTexCoords;
    float displacement = sin(aPos.x * time) * windStrength;
    gl_Position = projection * view * model * vec4(aPos.x + displacement, aPos.y , aPos.z, 1.0);
}
