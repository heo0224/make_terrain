#version 410 core

in vec3 teseColor;
out vec4 fragColor;

void main()
{
    fragColor = vec4(teseColor, 1.0);
}