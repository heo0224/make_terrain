#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include "common.h"
#include "camera.h"
#include "shader.h"
#include "texture.h"
#include "terrain.h"

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
    glm::mat4 getModelMatrix(glm::vec3 transl = glm::vec3(0.0f), glm::vec3 axis = glm::vec3(0.0f, 1.0f, 0.0f), float angleInDeg = 0.0f, glm::vec3 scale = glm::vec3(1.0f));
    glm::mat4 getViewMatrix();
    glm::mat4 getProjectionMatrix();

private:
    Context() {};
    bool init();

    std::unique_ptr<Camera> camera;
    std::unique_ptr<Shader> shader;
    std::unique_ptr<Shader> skyboxShader;
    std::unique_ptr<Shader> waterShader;
    std::unique_ptr<Terrain> terrain;
    std::shared_ptr<Texture> containerTexture;
    std::shared_ptr<Texture> grassGroundtexture;
    std::shared_ptr<CubemapTexture> skyboxTexture;

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
    bool wireFrameMode = false;

    unsigned int cubeVAO;
    unsigned int quadVAO;
    unsigned int skyBoxVAO;
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