#version 410 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 24) out;

in TESE_OUT {
    vec3 color;
    vec4 bottomPoint;
} gs_in[];

out GS_OUT {
    vec3 color;
} gs_out;

uniform bool showGround;

void addTriangle(vec4 v0, vec4 v1, vec4 v2, vec3 c0, vec3 c1, vec3 c2);
void addQuad(vec4 v0, vec4 v1, vec4 v2, vec4 v3, vec3 c0, vec3 c1, vec3 c2, vec3 c3);

void main()
{
    vec4 v0 = gl_in[0].gl_Position;
    vec4 v1 = gl_in[1].gl_Position;
    vec4 v2 = gl_in[2].gl_Position;
    vec3 c0 = gs_in[0].color;
    vec3 c1 = gs_in[1].color;
    vec3 c2 = gs_in[2].color;
    vec4 b0 = gs_in[0].bottomPoint;
    vec4 b1 = gs_in[1].bottomPoint;
    vec4 b2 = gs_in[2].bottomPoint;

    addTriangle(v0, v1, v2, c0, c1, c2);  // top triangle
    if (showGround) {
        addTriangle(b2, b1, b0, c2, c1, c0);  // bottom triangle (reverse winding order for correct face culling)
        addQuad(v0, v1, b1, b0, c0, c1, c1, c0);  // side 1
        addQuad(v1, v2, b2, b1, c1, c2, c2, c1);  // side 2
        addQuad(v2, v0, b0, b2, c2, c0, c0, c2);  // side 3
    }
}

void addTriangle(vec4 v0, vec4 v1, vec4 v2, vec3 c0, vec3 c1, vec3 c2)
{
    gl_Position = v0;
    gs_out.color = c0;
    EmitVertex();

    gl_Position = v1;
    gs_out.color = c1;
    EmitVertex();

    gl_Position = v2;
    gs_out.color = c2;
    EmitVertex();

    EndPrimitive();
}

void addQuad(vec4 v0, vec4 v1, vec4 v2, vec4 v3, vec3 c0, vec3 c1, vec3 c2, vec3 c3)
{
    // first triangle
    gl_Position = v0;
    gs_out.color = c0;
    EmitVertex();

    gl_Position = v1;
    gs_out.color = c1;
    EmitVertex();

    gl_Position = v2;
    gs_out.color = c2;
    EmitVertex();

    EndPrimitive();

    // second triangle
    gl_Position = v2;
    gs_out.color = c2;
    EmitVertex();

    gl_Position = v3;
    gs_out.color = c3;
    EmitVertex();

    gl_Position = v0;
    gs_out.color = c0;
    EmitVertex();

    EndPrimitive();
}
