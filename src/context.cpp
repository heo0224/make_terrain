#include "context.h"
#include "geometry_primitives.h"
#include "utils.h"
#include <imgui.h>

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
    skyboxShader = std::make_unique<Shader>("../shaders/shader_skybox.vs", "../shaders/shader_skybox.fs");
    skyboxShader->setInt("skyboxTexture1", 0);
    waterShader = std::make_unique<Shader>("../shaders/shader_water.vs", "../shaders/shader_water.fs");

    containerTexture = std::make_shared<Texture>("../assets/container.jpg");
    grassGroundtexture = std::make_shared<Texture>("../assets/grass_ground.jpg");

    // skybox
    std::vector<std::string> skyBoxFaces
    {
        "../assets/skybox/right.tga",
        "../assets/skybox/left.tga",
        "../assets/skybox/top.tga",
        "../assets/skybox/bottom.tga",
        "../assets/skybox/front.tga",
        "../assets/skybox/back.tga"
    };
    skyboxTexture = std::make_shared<CubemapTexture>(skyBoxFaces);

    skyBoxVAO = generatePositionVAO(skyBoxPositions, sizeof(skyBoxPositions));
    cubeVAO = generatePositionTextureVAO(cubePositionsTextures, sizeof(cubePositionsTextures));
    quadVAO = generatePositionTextureVAOWithEBO(quadPositionTextures, sizeof(quadPositionTextures), quadIndices, sizeof(quadIndices));

    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
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

    glm::mat4 projection = glm::perspective(glm::radians(camera->zoom), (float)this->width / (float)this->height, 0.1f, 100.0f);
    glm::mat4 view = camera->getViewMatrix();
    glm::mat4 model = glm::mat4(1.0f);
    shader->use();
    shader->setMat4("projection", projection);
    shader->setMat4("view", view);
    shader->setMat4("model", model);

    // cube
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, containerTexture->ID);
    glBindVertexArray(cubeVAO);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 1.0f, -2.0f));
    model = glm::rotate(model, glm::radians(20.0f), glm::vec3(1.0f, 0.3f, 0.5f));
    shader->setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // ground
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, grassGroundtexture->ID);
    glBindVertexArray(quadVAO);
    model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(grassGroundSize));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    shader->setMat4("model", model);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    //water
    glBindVertexArray(quadVAO);
    model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(waterSize));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, waterHeight));
    waterShader->use();
    waterShader->setMat4("projection", projection);
    waterShader->setMat4("view", view);
    waterShader->setMat4("model", model);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // skybox
    skyboxShader->use();
    glDepthFunc(GL_LEQUAL);
    view = glm::mat4(glm::mat3(camera->getViewMatrix()));
    skyboxShader->setMat4("view", view);
    skyboxShader->setMat4("projection", projection);

    glBindVertexArray(skyBoxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture->textureID);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
}

void Context::renderGUI() {
    if (ImGui::Begin("UI Window Example")) {
        ImGui::Text("Rendering Mode");
        if (ImGui::RadioButton("Fill", !wireFrameMode)) {
            wireFrameMode = false;
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Wireframe", wireFrameMode)) {
            wireFrameMode = true;
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        ImGui::Separator();
        ImGui::SliderFloat("grass ground size", &grassGroundSize, 10.0f, 60.0f);
        ImGui::SliderFloat("water height", &waterHeight, -1.0f, 1.0f);
        ImGui::Separator();
        if (ImGui::Button("reset camera")) {
            camera->reset();
        }
    }
    ImGui::End();
}