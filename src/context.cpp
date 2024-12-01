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
    shader->use();
    shader->setInt("texture0", 0);

    skyboxshader = std::make_unique<Shader>("../shaders/shader_skybox.vs", "../shaders/shader_skybox.fs");
    skyboxshader->setInt("skyboxTexture1", 0);

    watershader = std::make_unique<Shader>("../shaders/shader_water.vs", "../shaders/shader_water.fs");

    containertexture = std::make_shared<Texture>("../assets/container.jpg");
    grassGroundtexture = std::make_shared<Texture>("../assets/grass_ground.jpg");

    //skybox
    std::vector<std::string> skybox_faces
    {
        "../assets/skybox/right.tga",
        "../assets/skybox/left.tga",
        "../assets/skybox/top.tga",
        "../assets/skybox/bottom.tga",
        "../assets/skybox/front.tga",
        "../assets/skybox/back.tga"
    };
    skyboxTexture = std::make_shared<CubemapTexture>(skybox_faces);

    skyboxVAO = generatePositionVAO(skybox_positions, sizeof(skybox_positions));
    cubeVAO = generatePositionTextureVAO(cubePositionsTextures, sizeof(cubePositionsTextures));
    quadVAO = generatePositionTextureVAOWithEBO(quad_positions_textures, sizeof(quad_positions_textures), quad_indices, sizeof(quad_indices));

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
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, containertexture->ID);
    glBindVertexArray(cubeVAO);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 1.0f, -2.0f));
    model = glm::rotate(model, glm::radians(20.0f), glm::vec3(1.0f, 0.3f, 0.5f));
    shader->setMat4("view", view);
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
    watershader->use();
    watershader->setMat4("projection", projection);
    watershader->setMat4("view", view);
    watershader->setMat4("model", model);
    watershader->setFloat("textureTiling", 1.0f);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    shader->use();
    // cube
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, containertexture->ID);
    glBindVertexArray(cubeVAO);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 1.0f, -2.0f));
    model = glm::rotate(model, glm::radians(20.0f), glm::vec3(1.0f, 0.3f, 0.5f));
    shader->setMat4("view", view);
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

    // skybox
    skyboxshader->use();
    glDepthFunc(GL_LEQUAL);
    cubeview = glm::mat4(glm::mat3(view));
    skyboxshader->setMat4("view", cubeview);
    skyboxshader->setMat4("projection", projection);

    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture->textureID);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
}

void Context::renderGUI() {
    if (ImGui::Begin("UI Window Example")) {
        // if (ImGui::ColorEdit4("clear color", glm::value_ptr(clearColor))) {
        //     glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
        // }
        ImGui::Checkbox("lighting", &useLighting);
        ImGui::SameLine();
        ImGui::Checkbox("normal map", &useNormalMap);
        ImGui::SameLine();
        ImGui::Checkbox("shadow map", &useShadowMap);
        ImGui::SameLine();
        ImGui::Checkbox("PCF", &PCF);

        ImGui::SliderFloat("sun azimuth", &sun->azimuth, 0.0f, 360.0f);
        ImGui::SliderFloat("sun elevation", &sun->elevation, 5.0f, 90.0f);
        ImGui::SliderFloat("grass ground size", &grassGroundSize, 10.0f, 60.0f);
        ImGui::SliderFloat("water height", &waterHeight, -2.0f, 2.0f);
        ImGui::Separator();
        if (ImGui::Button("reset camera")) {
            camera->reset();
        }
    }
    ImGui::End();
}