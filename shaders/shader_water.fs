#version 330 core
out vec4 FragColor;

in Data{
    vec2 TexCoord;
    vec3 Normal;
    vec3 toCamera;
    vec3 fromLight;
    vec4 Pos;
} In;

uniform sampler2D texture0;

void main()
{
    vec2 ndc = (In.Pos.xy / In.Pos.w) * 0.5 + 0.5;
    // vec2 reflectionTexCoord = In.TexCoord;
    vec2 reflectionTexCoord = vec2(ndc.x, 1.0 - ndc.y);
    // reflectionTexCoord.x = clamp(reflectionTexCoord.x, 0.001, 0.999);
    // reflectionTexCoord.y = clamp(reflectionTexCoord.y, -0.999, -0.001);

    vec4 reflectionColor = texture(texture0, reflectionTexCoord);
    FragColor = reflectionColor;
    // FragColor = vec4(0.0, 0.0, 1.0, 1.0);
}