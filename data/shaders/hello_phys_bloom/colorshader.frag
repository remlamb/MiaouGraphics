#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D scene;
uniform sampler2D bloomBlur;

uniform float exposure;
uniform float bloomStrength = 0.04f;
uniform int programChoice;

uniform bool BnWFilter;
uniform float gamma = 1.8;

vec3 bloom_none()
{
    vec3 hdrColor = texture(scene, TexCoords).rgb;
    return hdrColor;
}

vec3 bloom_old()
{
    vec3 hdrColor = texture(scene, TexCoords).rgb;
    vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;
    return hdrColor + bloomColor; // additive blending
}

vec3 bloom_new()
{
    vec3 hdrColor = texture(scene, TexCoords).rgb;
    vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;
    return mix(hdrColor, bloomColor, bloomStrength); // linear interpolation
}

void main()
{
    // to bloom or not to bloom
    vec3 result = vec3(0.0);
    switch (programChoice)
    {
    case 1: result = bloom_none(); break;
    case 2: result = bloom_old(); break;
    case 3: result = bloom_new(); break;
    default:
        result = bloom_none(); break;
    }
    // tone mapping
    //result = vec3(1.0) - exp(-result * exposure);
    result = result / (result + vec3(1.0));
    //gamma correct
    result = pow(result, vec3(1.0 / gamma));
    FragColor = vec4(result, 1.0);

    if(BnWFilter){
      //FragColor = texture(scene, TexCoords);
      float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
      FragColor = vec4(average, average, average, 1.0);
    }
}
