#include "context.h"
#include "utils.h"
#include "geometry_primitives.h"
#include <imgui.h>

std::unique_ptr<Context> Context::create() {
    auto context = std::unique_ptr<Context>(new Context());
    if (!context->init())
        return nullptr;
    return std::move(context);
}

bool Context::init() {
    camera = std::make_unique<Camera>();
    light = std::make_unique<DirectionalLight>(this);
    skybox = std::make_unique<Skybox>(this);
    terrain = Terrain::createWithTessellation(this);

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
    if (!cameraMouseControlActivated)
        return;

    float xoffset = x - lastX;
    float yoffset = lastY - y; // reversed since y-coordinates go from bottom to top

    lastX = x;
    lastY = y;
    camera->processMouseMovement(xoffset, yoffset);
}

void Context::mouseButton(int button, int action, double x, double y) {
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        cameraMouseControlActivated = true;
        lastX = x;
        lastY = y;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
        cameraMouseControlActivated = false;
    }
}

void Context::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    terrain->render();
    skybox->render();
}

void Context::renderGUI() {
    if (ImGui::Begin("UI Window Example")) {
        if (ImGui::TreeNode("Rendering Mode")) {
            if (ImGui::RadioButton("Fill", !wireFrameMode)) {
                wireFrameMode = false;
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
            ImGui::SameLine();
            if (ImGui::RadioButton("Wireframe", wireFrameMode)) {
                wireFrameMode = true;
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            }
            ImGui::TreePop();
        }
        ImGui::Separator();

        if (ImGui::CollapsingHeader("Terrain")) {
            ImGui::SliderFloat("height offset", &terrain->heightOffset, -100.0f, 100.0f);
            ImGui::SliderFloat("height scale", &terrain->heightScale, 0.0f, 100.0f);
            ImGui::SliderFloat("horizontal scale", &terrain->horizontalScale, 0.01f, 0.1f);
            ImGui::SliderInt("min tess level", &terrain->minTessLevel, 2, terrain->maxTessLevel - 1);
            ImGui::SliderInt("max tess level", &terrain->maxTessLevel, terrain->minTessLevel + 1, 64);
            ImGui::SliderFloat("min distance", &terrain->minDistance, 1.0f, terrain->maxDistance);
            ImGui::SliderFloat("max distance", &terrain->maxDistance, terrain->minDistance, 100.0f);
        }
    }
    ImGui::End();
}
