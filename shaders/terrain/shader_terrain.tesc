#version 410
layout(vertices = 4) out;

in VS_OUT {
	vec2 texCoord;
} tesc_in[];

out TESC_OUT {
    vec2 texCoord;
} tesc_out[];

uniform mat4 model;
uniform mat4 view;

uniform int minTessLevel;
uniform int maxTessLevel;
uniform float minDistance;
uniform float maxDistance;

void main()
{
    // pass attributes through
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    tesc_out[gl_InvocationID].texCoord = tesc_in[gl_InvocationID].texCoord;

    // calculate tessellation levels
    if (gl_InvocationID == 0)
    {
        // transform each vertex into eye space
        vec4 eyeSpacePos00 = view * model * gl_in[0].gl_Position;
        vec4 eyeSpacePos01 = view * model * gl_in[1].gl_Position;
        vec4 eyeSpacePos10 = view * model * gl_in[2].gl_Position;
        vec4 eyeSpacePos11 = view * model * gl_in[3].gl_Position;

        // normalize distance from camera to [0, 1]
        float distance00 = clamp((abs(eyeSpacePos00.z)-minDistance) / (maxDistance-minDistance), 0.0, 1.0);
        float distance01 = clamp((abs(eyeSpacePos01.z)-minDistance) / (maxDistance-minDistance), 0.0, 1.0);
        float distance10 = clamp((abs(eyeSpacePos10.z)-minDistance) / (maxDistance-minDistance), 0.0, 1.0);
        float distance11 = clamp((abs(eyeSpacePos11.z)-minDistance) / (maxDistance-minDistance), 0.0, 1.0);

        // interpolate edge tessellation level based on closer vertex
        float tessLevel0 = mix( maxTessLevel, minTessLevel, min(distance10, distance00) );
        float tessLevel1 = mix( maxTessLevel, minTessLevel, min(distance00, distance01) );
        float tessLevel2 = mix( maxTessLevel, minTessLevel, min(distance01, distance11) );
        float tessLevel3 = mix( maxTessLevel, minTessLevel, min(distance11, distance10) );

        // set the corresponding outer edge tessellation levels
        gl_TessLevelOuter[0] = tessLevel0;
        gl_TessLevelOuter[1] = tessLevel1;
        gl_TessLevelOuter[2] = tessLevel2;
        gl_TessLevelOuter[3] = tessLevel3;

        // set the inner tessellation levels to the max of the two parallel edges
        gl_TessLevelInner[0] = max(tessLevel1, tessLevel3);
        gl_TessLevelInner[1] = max(tessLevel0, tessLevel2);
    }
}