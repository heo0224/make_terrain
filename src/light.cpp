#include "common.h"
#include "context.h"
#include "light.h"

glm::mat4 DirectionalLight::getLightViewMatrix() {
    glm::vec3 terrainCenter = glm::vec3(0.0f, context->terrain->heightOffset, 0.0f);  // center of the scene
    glm::vec3 lightPos = terrainCenter - this->direction * lightDistance;
    return glm::lookAt(lightPos, terrainCenter, glm::vec3(0, 1, 0));
}

glm::mat4 DirectionalLight::getLightProjectionMatrix() {
    return glm::ortho(-frustumSize, frustumSize, -frustumSize, frustumSize, 0.1f, 100000.0f);
}

glm::mat4 DirectionalLight::getLightSpaceMatrix() {
    glm::mat4 lightProjection = getLightProjectionMatrix();
    glm::mat4 lightView = getLightViewMatrix();
    return lightProjection * lightView;
}

void DirectionalLight::updateLightDir() {
    float azimuthRad = glm::radians(azimuth);
    float elevationRad = glm::radians(elevation);

    float x = cos(azimuthRad) * cos(elevationRad);
    float y = sin(elevationRad);
    float z = -sin(azimuthRad) * cos(elevationRad);
    direction = glm::vec3(-x, -y, -z);
}