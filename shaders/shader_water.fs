#version 330 core
out vec4 FragColor;

in Data{
    vec2 TexCoord;
    vec3 Normal;
    vec3 toCamera;
    vec3 fromLight;
    vec3 Pos;
} In;

uniform sampler2D texture0;

void main()
{
    // FragColor = texture(texture0, TexCoord);
    FragColor = vec4(0.0, 0.0, 1.0, 1.0);
}