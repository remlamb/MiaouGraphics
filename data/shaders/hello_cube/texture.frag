#version 310 es
precision highp float;

in vec2 texCoords;
layout(location = 0) out vec4 FragColor;

uniform sampler2D tex1;
uniform sampler2D tex2;

uniform float value;

void main()
{
    FragColor = mix(texture(tex1, texCoords), texture(tex2, texCoords), value);
}
