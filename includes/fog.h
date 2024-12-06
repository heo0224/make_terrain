#ifndef __FOG_H__
#define __FOG_H__

#include "common.h"
#include "shader.h"
#include "framebuffer.h"

class Context;

class Fog {
public:
    Fog(Context* context);
    void render();
    float fogDensity = 0.0f;
    glm::vec3 fogColor = glm::vec3(0.5f, 0.5f, 0.5f);
    float fogHeight = 1.0f;
    bool isLayeredFog = false;
private:
    void init();

    Context* context;
    std::unique_ptr<Shader> fogShader;
    unsigned int screenQuadVAO;
    glm::mat4 view;
    glm::mat4 projection;
};

#endif