#version 410 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 24) out;

in TESE_OUT {
    vec3 color;
    vec4 bottomPoint;
    vec4 fragPosLightSpace;
    vec3 normal;
} gs_in[];

out GS_OUT {
    vec3 color;
    vec4 fragPosLightSpace;
    vec3 normal;
} gs_out;

uniform bool showGround;
uniform bool renderToDepthMap;

void addTriangle(vec4 v0, vec4 v1, vec4 v2, int idx0, int idx1, int idx2);
void addQuad(vec4 v0, vec4 v1, vec4 v2, vec4 v3, int idx0, int idx1, int idx2, int idx3);
void emitVertexWithAttributes(vec4 pos, int idx);

void main()
{
    vec4 v0 = gl_in[0].gl_Position;
    vec4 v1 = gl_in[1].gl_Position;
    vec4 v2 = gl_in[2].gl_Position;
    vec4 b0 = gs_in[0].bottomPoint;
    vec4 b1 = gs_in[1].bottomPoint;
    vec4 b2 = gs_in[2].bottomPoint;

    addTriangle(v0, v1, v2, 0, 1, 2);  // top triangle
    if (showGround && !renderToDepthMap) {
        addTriangle(b2, b1, b0, 2, 1, 0);  // bottom triangle (reverse winding order for correct face culling)
        addQuad(v0, v1, b1, b0, 0, 1, 1, 0);  // side 1
        addQuad(v1, v2, b2, b1, 1, 2, 2, 1);  // side 2
        addQuad(v2, v0, b0, b2, 2, 0, 0, 2);  // side 3
    }
}

void addTriangle(vec4 v0, vec4 v1, vec4 v2, int idx0, int idx1, int idx2)
{
    emitVertexWithAttributes(v0, idx0);
    emitVertexWithAttributes(v1, idx1);
    emitVertexWithAttributes(v2, idx2);
    EndPrimitive();
}

void addQuad(vec4 v0, vec4 v1, vec4 v2, vec4 v3, int idx0, int idx1, int idx2, int idx3)
{
    // first triangle
    emitVertexWithAttributes(v0, idx0);
    emitVertexWithAttributes(v1, idx1);
    emitVertexWithAttributes(v2, idx2);
    EndPrimitive();

    // second triangle
    emitVertexWithAttributes(v2, idx2);
    emitVertexWithAttributes(v3, idx3);
    emitVertexWithAttributes(v0, idx0);
    EndPrimitive();
}

void emitVertexWithAttributes(vec4 pos, int idx) {
    gl_Position = pos;
    gs_out.color = gs_in[idx].color;
    gs_out.fragPosLightSpace = gs_in[idx].fragPosLightSpace;
    gs_out.normal = gs_in[idx].normal;
    EmitVertex();
}