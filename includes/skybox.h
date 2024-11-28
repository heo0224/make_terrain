#ifndef __SKYBOX_H__
#define __SKYBOX_H__

#include "common.h"
#include "shader.h"
#include "texture.h"

class Context;  // forward declaration

class Skybox {
public:
    Skybox(Context* context);
    void render();
private:
    Context* context;
    std::unique_ptr<Shader> shader;
    std::unique_ptr<CubemapTexture> texture;
    unsigned int VAO;

};

#endif