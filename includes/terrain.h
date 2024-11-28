#ifndef __TERRAIN_H__
#define __TERRAIN_H__

#include "common.h"
#include "shader.h"
#include "texture.h"

class Context;  // forward declaration

class Terrain {
public:
    static std::unique_ptr<Terrain> createWithTessellation(Context* context);
    static std::unique_ptr<Terrain> createWithoutTessellation(Context* context);
    void render();
    void resetVertices();

    float heightScale = 9.0f;
    float heightOffset = 0.0f;
    float horizontalScale = 0.03f;
    int numStrips = 20;
    int minTessLevel = 2;
    int maxTessLevel = 16;
    float minDistance = 1.0f;
    float maxDistance = 10.0f;
    bool showGround = true;

private:
    Terrain(Context* context) : context(context) {};
    void init();

    Context* context;
    std::unique_ptr<Shader> shader;
    std::unique_ptr<Texture> heightMap;
    std::unique_ptr<Texture> diffuseMap;
    unsigned int VAO;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
};

#endif  // __TERRAIN_H__