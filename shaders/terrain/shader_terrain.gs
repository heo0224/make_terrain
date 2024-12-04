#version 410 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 24) out;

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
    vec4 fragPosLightSpace;
    vec3 normal;
} gs_out;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;
uniform mat3 normalMatrix;
uniform bool showGround;
uniform bool renderToDepthMap;

void addTriangle(vec4 v0, vec4 v1, vec4 v2, int idx0, int idx1, int idx2);
void addQuad(vec4 v0, vec4 v1, vec4 v2, vec4 v3, int idx0, int idx1, int idx2, int idx3);
void emitVertexWithAttributes(vec4 pos, vec3 normal, int idx);

void main()
{
    bool isAdjacentToBorder = gs_in[0].isAdjacentToBorder || gs_in[1].isAdjacentToBorder || gs_in[2].isAdjacentToBorder;
    bool isCloseToBorder = gs_in[0].isCloseToBorder || gs_in[1].isCloseToBorder || gs_in[2].isCloseToBorder;
    if (isAdjacentToBorder)
        return;

    // world space positions
    vec4 v0 = gl_in[0].gl_Position;
    vec4 v1 = gl_in[1].gl_Position;
    vec4 v2 = gl_in[2].gl_Position;
    vec4 b0 = gs_in[0].bottomPoint;
    vec4 b1 = gs_in[1].bottomPoint;
    vec4 b2 = gs_in[2].bottomPoint;

    addTriangle(v0, v1, v2, 0, 1, 2);  // top triangle
    if (showGround && !renderToDepthMap && isCloseToBorder) {
        addTriangle(b2, b1, b0, 2, 1, 0);  // bottom triangle (reverse winding order for correct face culling)
        addQuad(v0, v1, b1, b0, 0, 1, 1, 0);  // side 1
        addQuad(v1, v2, b2, b1, 1, 2, 2, 1);  // side 2
        addQuad(v2, v0, b0, b2, 2, 0, 0, 2);  // side 3
    }
}

void addTriangle(vec4 v0, vec4 v1, vec4 v2, int idx0, int idx1, int idx2)
{
    vec3 normal;
    normal = normalize(cross(vec3(v2 - v0), vec3(v1 - v0)));
    normal = normalize(normalMatrix * normal);
    emitVertexWithAttributes(v0, normal, idx0);
    emitVertexWithAttributes(v1, normal, idx1);
    emitVertexWithAttributes(v2, normal, idx2);
    EndPrimitive();
}

void addQuad(vec4 v0, vec4 v1, vec4 v2, vec4 v3, int idx0, int idx1, int idx2, int idx3)
{
    vec3 normal;
    // first triangle
    normal = normalize(cross(vec3(v2 - v0), vec3(v1 - v0)));
    normal = normalize(normalMatrix * normal);
    emitVertexWithAttributes(v0, normal, idx0);
    emitVertexWithAttributes(v1, normal, idx1);
    emitVertexWithAttributes(v2, normal, idx2);
    EndPrimitive();

    // second triangle
    normal = normalize(cross(vec3(v3 - v0), vec3(v2 - v0)));
    normal = normalize(normalMatrix * normal);
    emitVertexWithAttributes(v2, normal, idx2);
    emitVertexWithAttributes(v3, normal, idx3);
    emitVertexWithAttributes(v0, normal, idx0);
    EndPrimitive();
}

void emitVertexWithAttributes(vec4 pos, vec3 normal, int idx) {
    if (renderToDepthMap)
        gl_Position = lightSpaceMatrix * pos;
    else
        gl_Position = projection * view * pos;
    gs_out.color = gs_in[idx].color;
    gs_out.fragPosLightSpace = lightSpaceMatrix * pos;
    gs_out.normal = normal;
    gl_ClipDistance[0] = gs_in[idx].clipDistance;
    EmitVertex();
}