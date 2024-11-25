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
    void renderGUI();
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
    std::unique_ptr<Shader> watershader;
    std::shared_ptr<Texture> containertexture;
    std::shared_ptr<Texture> grassGroundtexture;

    int width = WINDOW_WIDTH;
    int height = WINDOW_HEIGHT;
    bool cameraMouseControlActivated = false;
    float lastX = WINDOW_WIDTH / 2.0f;
    float lastY = WINDOW_HEIGHT / 2.0f;
    float deltaTime = 0.0f;
    float lastTime = 0.0f;

    glm::vec4 clearColor = glm::vec4(0.2f, 0.3f, 0.3f, 1.0f);

    float grassGroundSize = 20.0f;
    float waterSize = 20.0f;
    float waterHeight = 0.0f;

    unsigned int cubeVAO;
    unsigned int quadVAO;
};

#endif // __CONTEXT_H__