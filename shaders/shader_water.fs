#version 330 core
out vec4 FragColor;

in Data{
    vec2 TexCoord;
    vec3 Normal;
    vec3 toCamera;
    vec3 fromLight;
    vec4 Pos;
} In;

uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;
uniform float mixfactor;
void main()
{
    vec2 ndc = (In.Pos.xy / In.Pos.w) * 0.5 + 0.5;
    vec2 reflectionTexCoord = vec2(ndc.x, -ndc.y);
    vec2 refractionTexCoord = vec2(ndc.x, ndc.y);

    vec4 reflectionColor = texture(reflectionTexture, reflectionTexCoord);
    vec4 refractionColor = texture(refractionTexture, refractionTexCoord);
    // Debug individual textures (optional)
    // FragColor = reflectionColor; return; // Test reflection
    // FragColor = refractionColor; return; // Test refraction
    FragColor = mix(reflectionColor, refractionColor, mixfactor);
}