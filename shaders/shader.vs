#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 clipPlane;
void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0f);
	vec4 worldPosition = model * vec4(aPos, 1.0);
    gl_ClipDistance[0] = dot(worldPosition, clipPlane);
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}
