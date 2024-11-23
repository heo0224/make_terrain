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

#endif  // __TEXTURE_H__