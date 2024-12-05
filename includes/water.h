#ifndef __WATER_H__
#define __WATER_H__

#include "common.h"
#include "texture.h"
#include "shader.h"
#include "framebuffer.h"

class Context;  // forward declaration

class Water {
public:
    Water(Context* context);

    int width = 1024; // Framebuffer width
    int height = 1024; // Framebuffer height
    std::unique_ptr<Framebuffer> reflectionBuffer;
    std::unique_ptr<Framebuffer> refractionBuffer;
    void render();
    float WAVE_SPEED = 0.05f;
    float waterLevel = 0.3f;
    float waterSize = 100.0f;
    float tiling = 10.0f;
    bool useDUDV = true;
private:
    void init();
    Context* context;
    std::unique_ptr<Shader> waterShader;
    unsigned int waterVAO;
    std::unique_ptr<Texture> dudvMap;
    std::unique_ptr<Texture> normalMap;
};

#endif