#include "light.h"
#include "utils.h"

DirectionalLight::DirectionalLight(float azimuth, float elevation, glm::vec3 lightColor) {
    this->azimuth = azimuth;
    this->elevation = elevation;
    updateLightDir();
    this->lightColor = lightColor;
}

DirectionalLight::DirectionalLight(glm::vec3 lightDir, glm::vec3 lightColor) {
    this->lightDir = lightDir;
    this->lightColor = lightColor;
}

glm::mat4 DirectionalLight::getViewMatrix(glm::vec3 cameraPosition) {
    // directional light has no light position. Assume fake light position depending on camera position.
    float lightDistance = 15.0f;
    glm::vec3 lightPos = cameraPosition - this->lightDir * lightDistance;
    return glm::lookAt(lightPos, cameraPosition, glm::vec3(0, 1, 0));
}

glm::mat4 DirectionalLight::getProjectionMatrix() {
    // For simplicity, just use static projection matrix. (Actually we have to be more accurate with considering camera's frustum)
    return glm::ortho(-15.0f, 15.0f, -15.0f, 15.0f, 0.1f, 50.0f);
}

void DirectionalLight::updateLightDir() {
    this->lightDir = -glm::vec3(
        cos(glm::radians(azimuth)) * cos(glm::radians(elevation)),
        sin(glm::radians(elevation)),
        sin(glm::radians(azimuth)) * cos(glm::radians(elevation))
        );
}

// Processes input received from a mouse input system. Expects the offset value in both the x(azimuth) and y(elevation) direction.
void DirectionalLight::processKeyboard(float xoffset, float yoffset)
{
    // set elevation between 15 to 80 (degree)!
    this->azimuth += xoffset;
    this->elevation += yoffset;
    elevation = clamp(elevation, 15, 80);
    updateLightDir();
}
