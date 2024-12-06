#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D sceneBuffer;
uniform sampler2D depthMap;

uniform vec3 fogColor;
// uniform float fogMin;
// uniform float fogMax;
uniform float fogDensity;
uniform float nearPlane;
uniform float farPlane;

uniform mat4 view;
uniform mat4 invProjection;
uniform mat4 invView;
uniform vec2 screenSize;
uniform vec3 cameraPosition;
uniform float fogHeight;
uniform bool isLayeredFog;

float LinearizeDepth(float depth) {
  float z = depth * 2.0 - 1.0;
  return (2.0 * nearPlane * farPlane) /
         (farPlane + nearPlane - z * (farPlane - nearPlane));
}

vec3 getWorldSpacePosition() {
  // get NDC coordinates
  vec2 ndc = TexCoords * 2.0 - 1.0;
  float z = texture(depthMap, TexCoords).r * 2.0 - 1.0;

  // get clip space coordinates
  vec4 clipSpaceCoords = vec4(ndc, z, 1.0);

  // get view space coordinates
  vec4 viewSpaceCoords = invProjection * clipSpaceCoords;
  viewSpaceCoords /= viewSpaceCoords.w;

  // get world space coordinates
  vec4 worldSpaceCoords = invView * viewSpaceCoords;

  return worldSpaceCoords.xyz;
}

float CalculateFogFactor(float depth) {
  float linearizedDepth =
      LinearizeDepth(depth) / farPlane; // linearize and normalize depth
  float fogFactor = exp(-pow(linearizedDepth * fogDensity, 2.0f));
  if (depth > 0.999)
    return clamp(fogFactor, 0.2, 1.0);

  return clamp(fogFactor, 0.0, 1.0);
}

float CalculateLayerdFogFactor(vec3 worldSpaceCoords, float depth) {
  // project world space coordinates to xz plane
  vec3 worldSpaceCoordsProj = worldSpaceCoords;
  worldSpaceCoordsProj.y = 0.0;

  // project camera position to xz plane
  vec3 cameraPositionProj = cameraPosition;
  cameraPositionProj.y = 0.0;

  // calculate normalized distance from camera in xz plane
  float deltaD = length(cameraPositionProj - worldSpaceCoordsProj) / farPlane;

  float deltaY = 0.0;
  float densityIntegral = 0.0;

  if (cameraPosition.y > fogHeight) {     // camera is above fog
    if (worldSpaceCoords.y < fogHeight && depth < 0.999) { // fragment is below fog and not skybox
      deltaY = (fogHeight - worldSpaceCoords.y) / fogHeight;
      densityIntegral = 0.5 * deltaY * deltaY;
    }      // when fragment is above fog, density integral is 0
  } else { // camera is below fog
    if (worldSpaceCoords.y < fogHeight) { // fragment is below fog
      if (depth > 0.999) {
        return CalculateFogFactor(depth);
      } else {
        deltaY = abs(cameraPosition.y - worldSpaceCoords.y) / fogHeight;
        float deltaYCamera = (fogHeight - cameraPosition.y) / fogHeight;
        float densityIntegralCamera = 0.5 * deltaYCamera * deltaYCamera;
        float deltaYFragment = (fogHeight - worldSpaceCoords.y) / fogHeight;
        float densityIntegralFragment = 0.5 * deltaYFragment * deltaYFragment;
        densityIntegral = abs(densityIntegralFragment - densityIntegralCamera);
      }
    } else { // fragment is above fog
      if (depth > 0.999) {
        return CalculateFogFactor(depth);
      } else {
        deltaY = (fogHeight - cameraPosition.y) / fogHeight;
        densityIntegral = 0.5 * deltaY * deltaY;
      }
    }
  }

  float integratedFogDensity = 0.0;

  if (deltaY < 0.0001) {
    deltaY = 0.0001;
  }

  integratedFogDensity = sqrt(1.0 + pow(deltaD / deltaY, 2.0)) * densityIntegral;

  float fogFactor = exp(-fogDensity * integratedFogDensity);

  return clamp(fogFactor, 0.0, 1.0);
}

void main() {
  vec4 color = texture(sceneBuffer, TexCoords);
  float depth = texture(depthMap, TexCoords).r;

  float fogFactor = 1.0f;

  fogFactor = CalculateFogFactor(depth);

  if (!isLayeredFog)
    fogFactor = CalculateFogFactor(depth);
  else {
    vec3 worldSpaceCoords = getWorldSpacePosition();
    fogFactor = CalculateLayerdFogFactor(worldSpaceCoords, depth);
  }

  vec3 finalColor = mix(fogColor, color.rgb, fogFactor);

  FragColor = vec4(finalColor, color.a);
}