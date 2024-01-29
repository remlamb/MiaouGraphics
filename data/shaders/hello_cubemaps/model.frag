#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 Position;
in vec2 TexCoords;


uniform samplerCube skybox;
uniform vec3 cameraPos;
uniform int textureMode;
uniform sampler2D texture_diffuse1;

void main()
{
if(textureMode == 0){
vec4 texColor = texture(texture_diffuse1, TexCoords);
if(texColor.a < 0.1){
        discard;
}
FragColor = texture(texture_diffuse1, TexCoords);

}
if(textureMode == 1){
float ratio = 1.00 / 1.309;
vec3 I = normalize(Position - cameraPos);
vec3 R = refract(I, normalize(Normal), ratio);
FragColor = vec4(texture(skybox, R).rgb, 1.0);
}
if(textureMode == 2){
vec3 I = normalize(Position - cameraPos);
    vec3 R = reflect(I, normalize(Normal));
    FragColor = vec4(texture(skybox, R).rgb, 1.0);
}
}
