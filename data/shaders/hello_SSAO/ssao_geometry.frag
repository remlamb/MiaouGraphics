#version 330 core
layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedo;

in vec2 TexCoords;
in vec3 FragPos;
in mat3 tangentToViewMatrix;

// material parameters
uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;

uniform bool isCustomMaterial;
uniform float metallic;
uniform float roughness;
uniform float ao;

void main()
{
    // store the fragment position vector in the first gbuffer texture
    gPosition.rgb = FragPos;
    if(isCustomMaterial){
        gPosition.a = metallic;
    }
    else{
        gPosition.a = texture(metallicMap, TexCoords).r;
    }


    // also store the per-fragment normals into the gbuffer
    vec3 normalTangent = texture(normalMap, TexCoords).rgb;
    normalTangent = normalTangent * 2.0 - 1.0;
    gNormal.rgb = normalize(tangentToViewMatrix * normalTangent);
    if(isCustomMaterial){
        gNormal.a = roughness;
    }
    else{
        gNormal.a = texture(roughnessMap, TexCoords).r;
    }


    // and the diffuse per-fragment color
    gAlbedo.rgb = texture(albedoMap, TexCoords).rgb;
    if(isCustomMaterial){
        gAlbedo.a = ao;
    }
    else{
        gAlbedo.a = texture(aoMap, TexCoords).r;
    }
}
