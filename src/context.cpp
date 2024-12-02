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
    water = std::make_unique<Water>(this);
    depthMap = Framebuffer::create(1024, 1024, AttachmentType::DEPTH);
    sceneBuffer = Framebuffer::create(width, height, AttachmentType::COLOR);
    quadVAO = generatePositionTextureVAOWithEBO(quadPositionTextures, sizeof(quadPositionTextures), quadIndices, sizeof(quadIndices));
    depthQuadShader = std::make_unique<Shader>(
        "../shaders/debug/shader_depth_quad.vs",
        "../shaders/debug/shader_depth_quad.fs"
    );
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
    _renderToDepthMap();
    _renderToScreen();

    // render skybox at last separately
    skybox->render();
}

void Context::_renderToDepthMap() {
    if (!useShadow)
        return;

    depthMap->bind();
    renderToDepthMap = true;
    glViewport(0, 0, depthMap->width, depthMap->height);
    glClear(GL_DEPTH_BUFFER_BIT);
    _drawScene();
    depthMap->unbind();
    renderToDepthMap = false;
}

void Context::_renderToScreen() {
    assert(!renderToDepthMap);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _drawScene();
}

void Context::_renderToWater() {
    glEnable(GL_CLIP_DISTANCE0);
    // reflection
    water->reflectionBuffer->bind();
    glViewport(0, 0, water->reflectionBuffer->width, water->reflectionBuffer->height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderReflection = true;
    // flip camera
    float distance = 2.0f * (camera->position.y - waterLevel);
    camera->position.y -= distance;
    camera->invertPitch();
    terrain->render();
    skybox->render();
    // flip back
    camera->position.y += distance;
    camera->invertPitch();
    water->reflectionBuffer->unbind();
    
    // refraction
    water->refractionBuffer->bind();
    glViewport(0, 0, water->refractionBuffer->width, water->refractionBuffer->height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderReflection = false;
    terrain->render();
    skybox->render();
    water->refractionBuffer->unbind();    
    glDisable(GL_CLIP_DISTANCE0);
}

glm::vec4 Context::getClipPlane() {
    if (renderReflection)
        return glm::vec4(0.0f, 1.0f, 0.0f, -waterLevel);
    else
        return glm::vec4(0.0f, -1.0f, 0.0f, waterLevel);
}

void Context::_drawScene() {
    terrain->render();
    water->render();
}

void Context::renderGUI() {
    if (ImGui::Begin("UI Window Example")) {
        if (ImGui::CollapsingHeader("Rendering")) {
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

            if (ImGui::TreeNode("Lighting")) {
                if (ImGui::SliderFloat("azimuth", &light->azimuth, 0.0f, 360.0f))
                    light->updateLightDir();
                if (ImGui::SliderFloat("elevation", &light->elevation, 0.0f, 90.0f))
                    light->updateLightDir();
                ImGui::SliderFloat("frustum size", &light->frustumSize, 10.0f, 10000.0f);
                ImGui::SliderFloat("near plane", &light->nearPlane, 0.1f, light->farPlane - 0.1f);
                ImGui::SliderFloat("far plane", &light->farPlane, light->nearPlane + 0.1f, 1000.0f);
                ImGui::SliderFloat("light distance", &light->lightDistance, 0.5f, 500.0f);
                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Shadow Mapping")) {
                ImGui::Checkbox("use shadow", &useShadow);
                ImGui::SameLine();
                ImGui::Checkbox("use PCF", &usePCF);
                ImGui::SliderFloat("min shadow bias", &minShadowBias, 0.001f, maxShadowBias - 0.001f);
                ImGui::SliderFloat("max shadow bias", &maxShadowBias, minShadowBias + 0.001f, 0.1f);
                ImGui::SliderInt("num PCF samples", &numPCFSamples, 1, 16);
                ImGui::SliderFloat("PCF spreadness", &PCFSpreadness, 1.0 / 50000.0f, 1.0 / 500.0f, "%.5f");
                ImGui::TreePop();
            }
        }

        if (ImGui::CollapsingHeader("Terrain")) {
            ImGui::Checkbox("show ground", &terrain->showGround);
            ImGui::SliderFloat("height offset", &terrain->heightOffset, -5.0f, 5.0f);
            ImGui::SliderFloat("height scale", &terrain->heightScale, 0.0f, 100.0f);
            ImGui::SliderFloat("horizontal scale", &terrain->horizontalScale, 0.01f, 0.1f);
            ImGui::SliderInt("min tess level", &terrain->minTessLevel, 2, terrain->maxTessLevel - 1);
            ImGui::SliderInt("max tess level", &terrain->maxTessLevel, terrain->minTessLevel + 1, 64);
            ImGui::SliderFloat("min distance", &terrain->minDistance, 1.0f, terrain->maxDistance);
            ImGui::SliderFloat("max distance", &terrain->maxDistance, terrain->minDistance, 100.0f);
        }

        if (ImGui::CollapsingHeader("Water")) {
            ImGui::SliderFloat("water level", &waterLevel, -5.0f, 5.0f);
            ImGui::SliderFloat("mix factor", &mixFactor, 0.0f, 1.0f);
        }
    }
    ImGui::End();

    /*
    ImGui::SetNextWindowCollapsed(true, ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Depth Map")) {
        ImVec2 contentSize = ImGui::GetContentRegionAvail();
        int newWidth = (int)contentSize.x;
        int newHeight = (int)contentSize.y;

        // resize the framebuffer if needed
        if (newWidth != sceneBuffer->width || newHeight != sceneBuffer->height) {
            newWidth = newWidth > 0 ? newWidth : 1;   // Avoid zero size
            newHeight = newHeight > 0 ? newHeight : 1;
            sceneBuffer->resizeFramebuffer(newWidth, newHeight);
        }

        // render the scene to the framebuffer
        sceneBuffer->bind();
        glViewport(0, 0, sceneBuffer->width, sceneBuffer->height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        depthQuadShader->use();
        depthQuadShader->bindTexture("depthMap", depthMap.get());
        glBindVertexArray(quadVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        sceneBuffer->unbind();

        // display the framebuffer texture
        ImGui::Image(
            (ImTextureID)sceneBuffer->texture,
            contentSize,
            ImVec2(0.0f, 1.0f),
            ImVec2(1.0f, 0.0f)
        );
    }
    */
   /*
    ImGui::SetNextWindowCollapsed(true, ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Reflection")) {
        ImVec2 contentSize = ImGui::GetContentRegionAvail();
        int newWidth = (int)contentSize.x;
        int newHeight = (int)contentSize.y;

        // resize the framebuffer if needed
        if (newWidth != sceneBuffer->width || newHeight != sceneBuffer->height) {
            newWidth = newWidth > 0 ? newWidth : 1;   // Avoid zero size
            newHeight = newHeight > 0 ? newHeight : 1;
            sceneBuffer->resizeFramebuffer(newWidth, newHeight);
        }

        // render the scene to the framebuffer
        _renderToWater();

        // display the framebuffer texture
        ImGui::Image(
            (ImTextureID)water->reflectionBuffer->texture,
            contentSize,
            ImVec2(0.0f, 1.0f),
            ImVec2(1.0f, 0.0f)
        );
    }
    ImGui::End();
   */
    
}
