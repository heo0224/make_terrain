#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include "common.h"
#include "camera.h"
#include "shader.h"
#include "texture.h"
#include "terrain.h"
#include "skybox.h"
#include "light.h"
#include "framebuffer.h"

class Context {
public:
    static std::unique_ptr<Context> create();
    void render();
    void _renderToDepthMap();
    void _renderToScreen();
    void _drawScene();
    void renderGUI();
    void updateDeltaTime();
    void processInput(GLFWwindow* window);
    void reshape(int width, int height);
    void mouseMove(double x, double y);
    void mouseButton(int button, int action, double x, double y);
    glm::mat4 getModelMatrix(glm::vec3 transl = glm::vec3(0.0f), glm::vec3 axis = glm::vec3(0.0f, 1.0f, 0.0f), float angleInDeg = 0.0f, glm::vec3 scale = glm::vec3(1.0f));
    glm::mat4 getViewMatrix();
    glm::mat4 getProjectionMatrix();

    friend class DirectionalLight;
    friend class Terrain;

private:
    Context() {};
    bool init();

    std::unique_ptr<Camera> camera;
    std::unique_ptr<DirectionalLight> light;
    std::unique_ptr<Terrain> terrain;
    std::unique_ptr<Skybox> skybox;
    std::unique_ptr<Framebuffer> depthMap;

    int width = WINDOW_WIDTH;
    int height = WINDOW_HEIGHT;
    bool cameraMouseControlActivated = false;
    float lastX = WINDOW_WIDTH / 2.0f;
    float lastY = WINDOW_HEIGHT / 2.0f;
    float deltaTime = 0.0f;
    float lastTime = 0.0f;
    bool wireFrameMode = false;

    // shadow mapping
    bool renderToDepthMap = false;
    bool useShadow = false;
    bool usePCF = false;
    float minShadowBias = 0.005f;
    float maxShadowBias = 0.01f;
    int numPCFSamples = 8;
    float PCFSpreadness = 1.0f / 3000.0f;
};

inline glm::mat4 Context::getModelMatrix(glm::vec3 transl, glm::vec3 axis, float angleInDeg, glm::vec3 scale) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, transl);
    model = glm::rotate(model, glm::radians(angleInDeg), axis);
    model = glm::scale(model, scale);
    return model;
}

inline glm::mat4 Context::getViewMatrix() {
    return camera->getViewMatrix();
}

inline glm::mat4 Context::getProjectionMatrix() {
    return glm::perspective(glm::radians(camera->zoom), (float)this->width / (float)this->height, 0.1f, 100000.0f);
}

#endif // __CONTEXT_H__