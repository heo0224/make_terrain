#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

// out vec2 TexCoord;
out Data{
    vec2 TexCoord;
    vec3 toCamera;
    vec3 fromLight;
    vec4 Pos;
} Out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 lightPos;
uniform int tiling;

void main()
{
    vec4 worldPos = model * vec4(aPos, 1.0);
	Out.TexCoord = vec2(aTexCoord.x, aTexCoord.y);
    Out.Pos = projection * view * worldPos;
	gl_Position = projection * view * model * vec4(aPos, 1.0f);
}
