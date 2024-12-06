#version 410 core
layout(triangles) in;
layout (line_strip, max_vertices = 12) out;

in TESE_OUT {
    vec3 color;
    vec4 bottomPoint;
    float clipDistance;
    float bottomClipDistance;
    bool isCloseToBorder;
    bool isAdjacentToBorder;
} gs_in[];

out GS_OUT {
    vec3 color;
} gs_out;

const float MAGNITUDE = 0.6;
const vec3 normalColor = vec3(1.0, 1.0, 0.0);
const vec3 lightDirColor = vec3(1.0, 0.5, 0.0);

uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightDir;
uniform bool showNormals;
uniform bool showLightDirection;

void generateLine(vec4 v0, vec4 v1, vec3 color);

void main()
{
    bool isAdjacentToBorder = gs_in[0].isAdjacentToBorder || gs_in[1].isAdjacentToBorder || gs_in[2].isAdjacentToBorder;
    if (isAdjacentToBorder)
        return;

    // world space positions
    vec4 v0 = gl_in[0].gl_Position;
    vec4 v1 = gl_in[1].gl_Position;
    vec4 v2 = gl_in[2].gl_Position;
    vec3 normal = normalize(cross(vec3(v2 - v0), vec3(v1 - v0)));

    if (showNormals) {
        generateLine(v0, v0 + vec4(normal, 0.0) * MAGNITUDE, normalColor);
        generateLine(v1, v1 + vec4(normal, 0.0) * MAGNITUDE, normalColor);
        generateLine(v2, v2 + vec4(normal, 0.0) * MAGNITUDE, normalColor);
    }
    if (showLightDirection) {
        generateLine(v0, v0 + vec4(-lightDir, 0.0) * MAGNITUDE, lightDirColor);
        generateLine(v1, v1 + vec4(-lightDir, 0.0) * MAGNITUDE, lightDirColor);
        generateLine(v2, v2 + vec4(-lightDir, 0.0) * MAGNITUDE, lightDirColor);
    }
}

void generateLine(vec4 v0, vec4 v1, vec3 color) {
    gl_Position = projection * view * v0;
    gs_out.color = color;
    EmitVertex();
    gl_Position = projection * view * v1;
    gs_out.color = color;
    EmitVertex();
    EndPrimitive();
}
