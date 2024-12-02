#version 330 core
out vec4 FragColor;

in Data{
    vec2 TexCoord;
    vec3 toCamera;
    vec3 fromLight;
    vec4 Pos;
} In;

uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;
uniform sampler2D dudvMap;
uniform sampler2D normalMap;

uniform bool useDUDV;
uniform float mixfactor;
uniform float moveFactor;

const float waveStrength = 0.01;

void main()
{
    vec2 ndc = (In.Pos.xy / In.Pos.w) * 0.5 + 0.5;
    vec2 reflectionTexCoord = vec2(ndc.x, -ndc.y);
    vec2 refractionTexCoord = vec2(ndc.x, ndc.y);


    vec2 texCoord = In.TexCoord;
    if (useDUDV)
    {
        vec2 distortedTexCoord = texture(dudvMap, vec2(texCoord.x + moveFactor, texCoord.y)).rg*0.1;
        distortedTexCoord = texCoord + vec2(distortedTexCoord.x, distortedTexCoord.y + moveFactor);
        vec2 totalDistortion = (texture(dudvMap, distortedTexCoord).rg * 2.0 - 1.0) * waveStrength;
        reflectionTexCoord += totalDistortion;
        refractionTexCoord += totalDistortion;
        reflectionTexCoord.x = clamp(reflectionTexCoord.x, 0.001, 0.999);
        reflectionTexCoord.y = clamp(reflectionTexCoord.y, -0.999, -0.001);
        refractionTexCoord = clamp(refractionTexCoord, 0.001, 0.999);
    }
    vec4 reflectionColor = texture(reflectionTexture, reflectionTexCoord);
    vec4 refractionColor = texture(refractionTexture, refractionTexCoord);
    vec4 blue = vec4(0.0, 0.2, 0.8, 1.0);
    FragColor = mix(reflectionColor, refractionColor, mixfactor);
    FragColor = mix(FragColor, blue, 0.1);
}