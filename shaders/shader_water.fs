#version 330 core
out vec4 FragColor;

in Data{
    vec2 TexCoord;
    vec3 toCamera;
    vec3 fromLight;
    vec4 Pos;
} In;

uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;
uniform sampler2D dudvMap;
uniform sampler2D normalMap;

uniform vec3 lightColor;
uniform bool useDUDV;
uniform bool useNormalMap;
uniform bool useSpecular;

uniform float moveFactor;
uniform float tiling;
const float waveStrength = 0.01;

void main()
{
    vec2 ndc = (In.Pos.xy / In.Pos.w) * 0.5 + 0.5;
    vec2 reflectionTexCoord = vec2(ndc.x, -ndc.y);
    vec2 refractionTexCoord = vec2(ndc.x, ndc.y);


    vec2 texCoord = In.TexCoord * tiling;
    if (useDUDV)
    {
        vec2 distortedTexCoord = texture(dudvMap, vec2(texCoord.x + moveFactor, texCoord.y)).rg*0.1;
        texCoord = texCoord + vec2(distortedTexCoord.x, distortedTexCoord.y + moveFactor);
        vec2 totalDistortion = (texture(dudvMap, texCoord).rg * 2.0 - 1.0) * waveStrength;
        reflectionTexCoord += totalDistortion;
        refractionTexCoord += totalDistortion;
        reflectionTexCoord.x = clamp(reflectionTexCoord.x, 0.001, 0.999);
        reflectionTexCoord.y = clamp(reflectionTexCoord.y, -0.999, -0.001);
        refractionTexCoord = clamp(refractionTexCoord, 0.001, 0.999);
    }
    vec3 normal = vec3(0.0,1.0,0.0);
    if (useNormalMap)
    {
        normal = texture(normalMap, texCoord).rgb;
        normal = normalize(vec3(normal.x * 2.0 - 1.0, normal.z, normal.y*2.0 - 1.0));
    }

    vec3 reflectedLight = -reflect(normalize(In.fromLight), normal);
    float specular = max(dot(reflectedLight, normalize(In.toCamera)), 0.0);
    vec3 specularColor = vec3(0.8, 0.8, 0.8);
    vec3 specularHighlight = 0.5 * lightColor * pow(specular, 20);

    vec4 reflectionColor = texture(reflectionTexture, reflectionTexCoord);
    vec4 refractionColor = texture(refractionTexture, refractionTexCoord);
    vec4 blue = vec4(0.0, 0.2, 0.8, 1.0);
    vec3 up = vec3(0.0, 1.0, 0.0);
    float reflectiveFactor = dot(normalize(In.toCamera), up);
    // reflectiveFactor = pow(reflectiveFactor, 2);
    FragColor = mix(reflectionColor, refractionColor, reflectiveFactor);
    FragColor = mix(FragColor, blue, 0.1);
    if (useSpecular){
        FragColor = FragColor + vec4(specularHighlight, 1.0);
    }
}