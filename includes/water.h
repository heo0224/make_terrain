#ifndef __WATER_H__
#define __WATER_H__

#include "common.h"
#include "texture.h"
#include "shader.h"
#include "framebuffer.h"

class Context;  // forward declaration

class Water{
public:
    Water(Context* context);

    int width = 480; // Framebuffer width
    int height = 480; // Framebuffer height
    std::unique_ptr<Framebuffer> reflectionBuffer;
    std::unique_ptr<Framebuffer> refractionBuffer;
    float waterLevel;
    float waterSize;
    unsigned int tiling;
    void render();
private:
    void init(float waterLevel, float waterSize, float tiling);
    Context* context;
    std::unique_ptr<Shader> waterShader;
    unsigned int waterVAO;
    std::unique_ptr<Texture> dudvMap;
    std::unique_ptr<Texture> normalMap;
};

#endif