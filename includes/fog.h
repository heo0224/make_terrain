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
private:
    void init();

    Context* context;
    std::unique_ptr<Shader> fogShader;
    unsigned int screenQuadVAO;
    glm::vec3 fogColor = glm::vec3(0.5f, 0.5f, 0.5f);
};

#endif