#ifndef __FRAMEBUFFER_H__
#define __FRAMEBUFFER_H__

#include "common.h"

enum class AttachmentType {
    COLOR,
    DEPTH
};

enum class BindType {
    ALL,
    READ,
    DRAW
};

class Framebuffer {
public:
    static std::unique_ptr<Framebuffer> create(int width, int height, AttachmentType type);
    void bind(BindType type = BindType::ALL);
    void unbind();

    int width;
    int height;
    unsigned int texture;
private:
    Framebuffer() {};
    bool initWithColorAttachment(int width, int height);
    bool initWithDepthAttachment(int width, int height);
    unsigned int FBO;
};

#endif // __FRAMEBUFFER_H__