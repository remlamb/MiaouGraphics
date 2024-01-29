#version 300 es
precision highp float;

layout (location = 0) in vec2 aPos;

out vec3 fragColor;


void main() {
    gl_Position = vec4(aPos, 0.0, 1.0);
    fragColor = vec3((aPos.x+1.0)/2.0, (aPos.y+1.0)/2.0, (aPos.x+1.0)/2.0);
}
