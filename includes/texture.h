#ifndef __TEXTURE_H__
#define __TEXTURE_H__

class Texture {
public:
    unsigned int ID;
    int width;
    int height;
    int channels;

    Texture(const char* filePath);
};

class CubemapTexture {
public:
    unsigned int textureID;
    int width;
    int height;
    int channels;

    CubemapTexture(const std::vector<std::string>& faces);
};

class DepthMapTexture {
public:
    unsigned int ID;
    unsigned int depthMapFBO;
    int width;
    int height;

    DepthMapTexture(int shadow_width, int shadow_height);
};

#endif  // __TEXTURE_H__