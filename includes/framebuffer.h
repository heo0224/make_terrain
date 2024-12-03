#ifndef __FRAMEBUFFER_H__
#define __FRAMEBUFFER_H__

#include "common.h"

enum class AttachmentType {
    COLOR,
    DEPTH,
    COLOR_AND_DEPTH
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
    void resizeFramebuffer(int width, int height);

    int width;
    int height;
    unsigned int texture;
    unsigned int colorTexture;
    unsigned int depthTexture;
private:
    Framebuffer() {};
    bool initWithColorAttachment(int width, int height);
    bool initWithDepthAttachment(int width, int height);
    bool initWithColorAndDepthAttachment(int width, int height);
    unsigned int FBO;
    unsigned int RBO;
};

#endif // __FRAMEBUFFER_H__