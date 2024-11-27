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
    sun = std::make_shared<DirectionalLight>(30.0f, 30.0f, glm::vec3(0.8f));
    
    shader = std::make_unique<Shader>("../shaders/shader.vs", "../shaders/shader.fs");
    skyboxShader = std::make_unique<Shader>("../shaders/shader_skybox.vs", "../shaders/shader_skybox.fs");
    waterShader = std::make_unique<Shader>("../shaders/shader_water.vs", "../shaders/shader_water.fs");
    containerTexture = std::make_shared<Texture>("../assets/container.jpg");
    grassGroundtexture = std::make_shared<Texture>("../assets/grass_ground.jpg");
    skyboxTexture = std::make_shared<CubemapTexture>(
        std::vector<std::string> {
        "../assets/skybox/right.tga",
            "../assets/skybox/left.tga",
            "../assets/skybox/top.tga",
            "../assets/skybox/bottom.tga",
            "../assets/skybox/front.tga",
            "../assets/skybox/back.tga"
    }
    );


    skyBoxVAO = generatePositionVAO(skyBoxPositions, sizeof(skyBoxPositions));
    cubeVAO = generatePositionTextureVAO(cubePositionsTextures, sizeof(cubePositionsTextures));
    quadVAO = generatePositionTextureVAOWithEBO(quadPositionTextures, sizeof(quadPositionTextures), quadIndices, sizeof(quadIndices));

    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glEnable(GL_DEPTH_TEST);

    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO); 
    
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        SPDLOG_ERROR("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

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
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    sun->updateLightDir();
    
    glEnable(GL_CLIP_DISTANCE0);
    // flip camera
    float distance = 2 * (camera->position.y - waterHeight);
    camera->position.y -= distance;
    camera->invertPitch();

    glm::mat4 projection = glm::perspective(glm::radians(camera->zoom), (float)this->width / (float)this->height, 0.1f, 100.0f);
    glm::mat4 view = camera->getViewMatrix();
    glm::mat4 model = glm::mat4(1.0f);
    shader->use();
    shader->setMat4("projection", projection);
    shader->setMat4("view", view);
    shader->setMat4("model", model);
    shader->setVec4("clipPlane", glm::vec4(0.0f, 1.0f, 0.0f, -waterHeight));
    shader->use();
    // cube
    glBindVertexArray(cubeVAO);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 1.0f, -2.0f));
    model = glm::rotate(model, glm::radians(20.0f), glm::vec3(1.0f, 0.3f, 0.5f));
    shader->bindTexture("texture0", containerTexture.get());
    shader->setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    // ground
    glBindVertexArray(quadVAO);
    model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(grassGroundSize));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    shader->setMat4("model", model);
    shader->bindTexture("texture0", grassGroundtexture.get());
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // skybox
    skyboxshader->use();
    glDepthFunc(GL_LEQUAL);
    glm::mat4 cubeview = glm::mat4(glm::mat3(view));
    skyboxshader->setMat4("view", cubeview);
    skyboxshader->setMat4("projection", projection);

    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture->textureID);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glBindVertexArray(0);
    glDepthFunc(GL_LESS);

    
    glDisable(GL_CLIP_DISTANCE0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //water
    camera->position.y += distance;
    camera->invertPitch();
    view = camera->getViewMatrix();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glBindVertexArray(quadVAO);
    model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(waterSize, 1.0f, waterSize));
    model = glm::translate(model, glm::vec3(0.0f, waterHeight, 0.0f));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, waterHeight));
    waterShader->use();
    waterShader->setMat4("projection", projection);
    waterShader->setMat4("view", view);
    waterShader->setMat4("model", model);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


    // skybox
    glDepthFunc(GL_LEQUAL);
    glBindVertexArray(skyBoxVAO);
    view = glm::mat4(glm::mat3(camera->getViewMatrix()));
    skyboxShader->use();
    skyboxShader->setMat4("view", view);
    skyboxShader->setMat4("projection", projection);
    skyboxShader->bindCubemapTexture("skyboxTexture1", skyboxTexture.get());
    glDrawArrays(GL_TRIANGLES, 0, 36);
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
        ImGui::SliderFloat("water height", &waterHeight, -2.0f, 2.0f);
        ImGui::Separator();
        if (ImGui::Button("reset camera")) {
            camera->reset();
        }
    }
    ImGui::End();
}