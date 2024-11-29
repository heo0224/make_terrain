#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "common.h"

class Context;  // forward declaration

class DirectionalLight {
public:
    DirectionalLight(Context* context);
    void updateLightDir();
    glm::mat4 getLightViewMatrix();
    glm::mat4 getLightProjectionMatrix();
    glm::mat4 getLightSpaceMatrix();

    float azimuth = 30.0f;
    float elevation = 30.0f;
    float lightDistance = 30.0f;
    float frustumSize = 1000.0f;
    glm::vec3 color = glm::vec3(1.0f);
    glm::vec3 direction = glm::vec3(0.0f, 0.0f, 1.0f);
private:
    Context* context;
};

#endif // __LIGHT_H__