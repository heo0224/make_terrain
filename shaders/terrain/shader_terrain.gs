#version 410 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 24) out;

in vec3 teseColor[];
in vec4 bottomPoint[];
out vec3 geoColor;

void addTriangle(vec4 v0, vec4 v1, vec4 v2, vec3 c0, vec3 c1, vec3 c2);
void addQuad(vec4 v0, vec4 v1, vec4 v2, vec4 v3, vec3 c0, vec3 c1, vec3 c2, vec3 c3);

void main()
{
    vec4 v0 = gl_in[0].gl_Position;
    vec4 v1 = gl_in[1].gl_Position;
    vec4 v2 = gl_in[2].gl_Position;
    vec3 c0 = teseColor[0];
    vec3 c1 = teseColor[1];
    vec3 c2 = teseColor[2];
    vec4 b0 = bottomPoint[0];
    vec4 b1 = bottomPoint[1];
    vec4 b2 = bottomPoint[2];

    addTriangle(v0, v1, v2, c0, c1, c2);  // top triangle
    addTriangle(b2, b1, b0, c2, c1, c0);  // bottom triangle (reverse winding order for correct face culling)
    addQuad(v0, v1, b1, b0, c0, c1, c1, c0);  // side 1
    addQuad(v1, v2, b2, b1, c1, c2, c2, c1);  // side 2
    addQuad(v2, v0, b0, b2, c2, c0, c0, c2);  // side 3
}

void addTriangle(vec4 v0, vec4 v1, vec4 v2, vec3 c0, vec3 c1, vec3 c2)
{
    gl_Position = v0;
    geoColor = c0;
    EmitVertex();

    gl_Position = v1;
    geoColor = c1;
    EmitVertex();

    gl_Position = v2;
    geoColor = c2;
    EmitVertex();

    EndPrimitive();
}

void addQuad(vec4 v0, vec4 v1, vec4 v2, vec4 v3, vec3 c0, vec3 c1, vec3 c2, vec3 c3)
{
    // first triangle
    gl_Position = v0;
    geoColor = c0;
    EmitVertex();

    gl_Position = v1;
    geoColor = c1;
    EmitVertex();

    gl_Position = v2;
    geoColor = c2;
    EmitVertex();

    EndPrimitive();

    // second triangle
    gl_Position = v2;
    geoColor = c2;
    EmitVertex();

    gl_Position = v3;
    geoColor = c3;
    EmitVertex();

    gl_Position = v0;
    geoColor = c0;
    EmitVertex();

    EndPrimitive();
}
