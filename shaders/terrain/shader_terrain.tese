#version 410 core
// layout( quads, equal_spacing, ccw) in;
layout (quads, fractional_odd_spacing, ccw) in;

in TESC_OUT {
    vec2 texCoord;
} tese_in[];

out TESE_OUT {
    vec3 color;
    vec4 bottomPoint;
} tese_out;

uniform sampler2D heightMap;
uniform sampler2D diffuseMap;
uniform float heightScale;
uniform float heightOffset;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // get patch coordinate
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    // retrieve control point texture coordinates
    vec2 t00 = tese_in[0].texCoord;
    vec2 t01 = tese_in[1].texCoord;
    vec2 t10 = tese_in[2].texCoord;
    vec2 t11 = tese_in[3].texCoord;

    // bilinearly interpolate texture coordinate across patch
    vec2 t0 = (t01 - t00) * u + t00;
    vec2 t1 = (t11 - t10) * u + t10;
    vec2 texCoord = (t1 - t0) * v + t0;

    // lookup texel at patch coordinate for height and scale + shift as desired
    float height = texture(heightMap, texCoord).y * heightScale + heightOffset;

    // ----------------------------------------------------------------------
    // retrieve control point position coordinates
    vec4 p00 = gl_in[0].gl_Position;
    vec4 p01 = gl_in[1].gl_Position;
    vec4 p10 = gl_in[2].gl_Position;
    vec4 p11 = gl_in[3].gl_Position;

    // compute patch surface normal
    vec4 uVec = p01 - p00;
    vec4 vVec = p10 - p00;
    vec4 normal = normalize( vec4(cross(vVec.xyz, uVec.xyz), 0) );

    // bilinearly interpolate position coordinate across patch
    vec4 p0 = (p01 - p00) * u + p00;
    vec4 p1 = (p11 - p10) * u + p10;
    vec4 p = (p1 - p0) * v + p0;

    // displace point along normal
    vec4 tp = p + normal * height;
    vec4 bp = p + normal * heightOffset;

    // ----------------------------------------------------------------------
    // output patch point position in clip space
    gl_Position = projection * view * model * tp;
    tese_out.color = texture(diffuseMap, texCoord).rgb;
    tese_out.bottomPoint = projection * view * model * bp;
}