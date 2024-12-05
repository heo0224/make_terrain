#version 410 core

in GS_OUT {
    vec3 color;
    vec4 fragPosLightSpace;
    vec3 normal;
} fs_in;

out vec4 fragColor;

uniform sampler2D depthMap;
uniform bool renderToDepthMap;
uniform bool useLighting;
uniform bool useShadow;
uniform bool usePCF;
uniform vec3 lightDir;
uniform float ambientStrength;
uniform float minShadowBias;
uniform float maxShadowBias;
uniform int numPCFSamples;
uniform float PCFSpreadness;

float calculateShadow(vec4 fragPosLightSpace);
float random(vec3 seed, int i);

void main()
{
    if (renderToDepthMap) {
        return;
    }

    vec3 color = fs_in.color;
    float shadow = calculateShadow(fs_in.fragPosLightSpace);
    if (!useLighting) {
        fragColor = vec4(ambientStrength * color + color * (1.0 - ambientStrength) * (1.0 - shadow), 1.0);
        return;
    }

    vec3 ambient = ambientStrength * color;
    vec3 diffuse = max(dot(normalize(fs_in.normal), -lightDir), 0.0) * color * (1.0 - ambientStrength);
    fragColor = vec4(ambient + (1.0 - shadow) * diffuse, 1.0);
}

const vec2 poissonDisk[16] = vec2[]( 
   vec2( -0.94201624, -0.39906216 ), 
   vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), 
   vec2( 0.34495938, 0.29387760 ), 
   vec2( -0.91588581, 0.45771432 ), 
   vec2( -0.81544232, -0.87912464 ), 
   vec2( -0.38277543, 0.27676845 ), 
   vec2( 0.97484398, 0.75648379 ), 
   vec2( 0.44323325, -0.97511554 ), 
   vec2( 0.53742981, -0.47373420 ), 
   vec2( -0.26496911, -0.41893023 ), 
   vec2( 0.79197514, 0.19090188 ), 
   vec2( -0.24188840, 0.99706507 ), 
   vec2( -0.81409955, 0.91437590 ), 
   vec2( 0.19984126, 0.78641367 ), 
   vec2( 0.14383161, -0.14100790 ) 
);

float calculateShadow(vec4 fragPosLightSpace) {
    if (!useShadow) {
        return 0.0;
    }
    
    // perform perspective divide and normalize to [0,1] range
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    // calculate shadow
    float shadow = 0.0;
    float diff = 1.0 - dot(normalize(fs_in.normal), -lightDir);
    float bias = max(maxShadowBias * diff, minShadowBias);
    if (usePCF) {
        for (int i = 0; i < numPCFSamples; i++) {
            int idx = int(16.0 * random(floor(fragPosLightSpace.xyz * 1000.0), i)) % 16;
            float closestDepth = texture(depthMap, projCoords.xy + poissonDisk[idx] * PCFSpreadness).r;
            float currentDepth = projCoords.z;
            if (currentDepth - bias > closestDepth)
                shadow += (1.0 / float(numPCFSamples));
            shadow = clamp(shadow, 0.0, 1.0);
        }
    }
    else {
        float closestDepth = texture(depthMap, projCoords.xy).r;
        float currentDepth = projCoords.z;
        shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    }
    return shadow;
}

float random(vec3 seed, int i) {
    vec4 seed4 = vec4(seed, i);
    float dot_product = dot(seed4, vec4(12.9898, 78.233, 45.164, 94.673));
    return fract(sin(dot_product) * 43758.5453);
}