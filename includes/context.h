#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include "common.h"

class Context {
public:
    static std::unique_ptr<Context> create();
    void render();
    void processInput(GLFWwindow* window);
    void reshape(int width, int height);
    void mouseMove(double x, double y);
    void mouseButton(int button, int action, double x, double y);

private:
    Context() {};
    bool init();

    int width = 1280;
    int height = 720;
};

#endif // __CONTEXT_H__