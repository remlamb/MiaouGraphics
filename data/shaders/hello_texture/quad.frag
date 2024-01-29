#version 300 es
precision highp float;

in vec2 TexCoords;
layout(location = 0) out vec4 outColor;

uniform sampler2D tex;

void main()
{
    outColor = texture(tex, TexCoords);
}
