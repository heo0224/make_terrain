#include "context.h"
#include "geometry_primitives.h"
#include "utils.h"

std::unique_ptr<Context> Context::create() {
    auto context = std::unique_ptr<Context>(new Context());
    if (!context->init())
        return nullptr;
    return std::move(context);
}

bool Context::init() {
    camera = std::make_unique<Camera>();
    shader = std::make_unique<Shader>("../shaders/shader.vs", "../shaders/shader.fs");
    shader->use();
    shader->setInt("texture0", 0);
    texture = std::make_shared<Texture>("../assets/container.jpg");
    cubeVAO = generatePositionTextureVAO(cubePositionsTextures, sizeof(cubePositionsTextures));

    glEnable(GL_DEPTH_TEST);
    return true;
}

void Context::updateDeltaTime() {
    float currentTime = static_cast<float>(glfwGetTime());
    deltaTime = currentTime - lastTime;
    lastTime = currentTime;
}

void Context::processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera->processKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera->processKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera->processKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera->processKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera->processKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera->processKeyboard(DOWN, deltaTime);
}

void Context::reshape(int width, int height) {
    this->width = width;
    this->height = height;
    glViewport(0, 0, width, height);
}

void Context::mouseMove(double x, double y) {
    if (firstMouse)
    {
        lastX = x;
        lastY = y;
        firstMouse = false;
    }
    float xoffset = x - lastX;
    float yoffset = lastY - y; // reversed since y-coordinates go from bottom to top

    lastX = x;
    lastY = y;
    camera->processMouseMovement(xoffset, yoffset);
}

void Context::mouseButton(int button, int action, double x, double y) {
    return;
}

void Context::render() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 projection = glm::perspective(glm::radians(camera->zoom), (float)this->width / (float)this->height, 0.1f, 100.0f);
    glm::mat4 view = camera->getViewMatrix();
    glm::mat4 model = glm::mat4(1.0f);
    shader->use();
    shader->setMat4("projection", projection);
    shader->setMat4("view", view);
    shader->setMat4("model", model);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->ID);
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    return;
}
