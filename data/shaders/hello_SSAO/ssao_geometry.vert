#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;

out vec3 FragPos;
out vec2 TexCoords;
out mat3 tangentToViewMatrix;

uniform bool invertedNormals;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 viewPos = view * model * vec4(aPos, 1.0);
    FragPos = viewPos.xyz;
    TexCoords = aTexCoords;

    mat3 normalMatrix = transpose(inverse(mat3(view * model)));
    vec3 T = normalize(normalMatrix * normalize(aTangent));
    vec3 N = normalize(normalMatrix * normalize(aNormal));
    T = normalize(T - dot(T, N) * N); //reorthogonalize the tangent
    vec3 B = normalize(cross(N, T));

    tangentToViewMatrix = mat3(T, B, N); // Don't inverse the TBN matrix.



    gl_Position = projection * viewPos;
}
