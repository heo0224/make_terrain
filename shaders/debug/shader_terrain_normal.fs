#version 410 core

in GS_OUT {
    vec3 color;
} fs_in;

out vec4 fragColor;


void main()
{
    fragColor = vec4(fs_in.color, 1.0);
}
