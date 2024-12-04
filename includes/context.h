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
#include "water.h"
#include "fog.h"

class Context {
public:
    static std::unique_ptr<Context> create();
    void render();
    void _renderToShadowFramebuffer();
    void _renderToFogFramebuffer();
    void _renderToWaterFramebuffer();
    void _renderToAntiAliasingScreenBuffer();
    void _renderToScreen();
    void renderGUI();
    void updateDeltaTime();
    void processInput(GLFWwindow* window);
    void reshape(int width, int height);
    void mouseMove(double x, double y);
    void mouseButton(int button, int action, double x, double y);
    glm::mat4 getModelMatrix(glm::vec3 transl = glm::vec3(0.0f), glm::vec3 axis = glm::vec3(0.0f, 1.0f, 0.0f), float angleInDeg = 0.0f, glm::vec3 scale = glm::vec3(1.0f));
    glm::mat4 getViewMatrix();
    glm::mat4 getProjectionMatrix();
    glm::vec3 getCameraPosition(); // added for Water class
    glm::vec4 getClipPlane();

    friend class DirectionalLight;
    friend class Terrain;
    friend class Water;
    friend class Fog;
private:
    Context() {};
    bool init();

    std::unique_ptr<Camera> camera;
    std::unique_ptr<DirectionalLight> light;
    std::unique_ptr<Terrain> terrain;
    std::unique_ptr<Skybox> skybox;
    std::unique_ptr<Water> water;
    std::unique_ptr<Fog> fog;
    std::unique_ptr<Framebuffer> depthMap;
    std::unique_ptr<Framebuffer> fogScreenBuffer;
    std::unique_ptr<Framebuffer> antiAliasingScreenBuffer;
    std::unique_ptr<Shader> depthQuadShader;
    std::unique_ptr<Shader> FXAAShader;
    unsigned int screenQuadVAO;

    int width = WINDOW_WIDTH;
    int height = WINDOW_HEIGHT;
    bool cameraMouseControlActivated = false;
    float lastX = WINDOW_WIDTH / 2.0f;
    float lastY = WINDOW_HEIGHT / 2.0f;
    float deltaTime = 0.0f;
    float lastTime = 0.0f;

    // flags
    bool isRenderingToDepthMap = false;
    bool isRenderingReflection = false;

    // rendering options
    bool wireFrameMode = false;
    bool renderTerrain = true;
    bool renderWater = true;
    bool renderFog = true;
    bool renderFogSaved = true;
    bool useAntiAliasing = false;
    bool useAntiAliasingSaved = false;
    bool showLightDirection = false;

    // shadow mapping
    bool useShadow = false;
    bool usePCF = false;
    float minShadowBias = 0.005f;
    float maxShadowBias = 0.01f;
    int numPCFSamples = 8;
    float PCFSpreadness = 1.0f / 3000.0f;

    // anti-aliasing (FXAA)
    float lumaThreshold = 0.5f;
    float mulReduce = 1.0f / 8.0f;
    float minReduce = 1.0f / 128.0f;
    float maxSpan = 8.0f;
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

inline glm::vec3 Context::getCameraPosition() {
    return camera->position;
}

#endif // __CONTEXT_H__