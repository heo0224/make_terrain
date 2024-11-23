#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include "common.h"
#include "camera.h"
#include "shader.h"
#include "texture.h"

class Context {
public:
    static std::unique_ptr<Context> create();
    void render();
    void updateDeltaTime();
    void processInput(GLFWwindow* window);
    void reshape(int width, int height);
    void mouseMove(double x, double y);
    void mouseButton(int button, int action, double x, double y);

private:
    Context() {};
    bool init();

    std::unique_ptr<Camera> camera;
    std::unique_ptr<Shader> shader;
    std::shared_ptr<Texture> texture;

    int width = WINDOW_WIDTH;
    int height = WINDOW_HEIGHT;

    bool firstMouse = true;
    float lastX = WINDOW_WIDTH / 2.0f;
    float lastY = WINDOW_HEIGHT / 2.0f;
    float deltaTime = 0.0f;
    float lastTime = 0.0f;

    unsigned int cubeVAO;
};

#endif // __CONTEXT_H__