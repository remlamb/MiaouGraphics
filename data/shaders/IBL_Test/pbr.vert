#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aTangent;

out vec3 fragViewPos;
out vec2 texCoords;
// Send the TBN matrix to the g_buffer.frag shader to transform the normal from
// tangent space to world space. Otherwise we would need to send the normals and tangents in
// tangent space and to create the TBN matrix in the lighting fragment shader which is not very performant.
out mat3 tangentToViewMatrix;

struct Transform{
  mat4 model;
  mat4 view;
  mat4 projection;
};

uniform Transform transform;
uniform mat4 viewNormalMatrix;

void main() {
    mat3 viewNormalMatrix = mat3(viewNormalMatrix);
    vec3 T = normalize(viewNormalMatrix * normalize(aTangent));
    vec3 N = normalize(viewNormalMatrix * normalize(aNormal));
    T = normalize(T - dot(T, N) * N); //reorthogonalize the tangent
    vec3 B = normalize(cross(N, T));

    tangentToViewMatrix = mat3(T, B, N); // Don't inverse the TBN matrix.

    vec4 viewPos = transform.view * transform.model * vec4(aPos, 1.0);
    fragViewPos = viewPos.xyz;
    texCoords = aTexCoords;

    gl_Position = transform.projection * viewPos;
}
