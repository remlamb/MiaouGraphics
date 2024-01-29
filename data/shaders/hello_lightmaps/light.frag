#version 310 es
precision highp float;

in vec3 Normal;

uniform vec3 lightPos;

out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0);
}
