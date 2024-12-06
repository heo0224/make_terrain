#include "context.h"
#include "utils.h"
#include "geometry_primitives.h"
#include <filesystem>
#include <imgui.h>

namespace fs = std::filesystem;

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
    fog = std::make_unique<Fog>(this);
    depthMap = Framebuffer::create(1024, 1024, AttachmentType::DEPTH);
    antiAliasingScreenBuffer = Framebuffer::create(width, height, AttachmentType::COLOR);
    fogScreenBuffer = Framebuffer::create(width, height, AttachmentType::COLOR_AND_DEPTH);
    screenQuadVAO = generatePositionTextureVAO(screenQuadVertices, sizeof(screenQuadVertices));
    depthQuadShader = std::make_unique<Shader>(
        "../shaders/debug/shader_depth_quad.vs",
        "../shaders/debug/shader_depth_quad.fs"
    );
    FXAAShader = std::make_unique<Shader>(
        "../shaders/shader_fxaa.vs",
        "../shaders/shader_fxaa.fs"
    );

    // load terrain directories
    fs::path baseDir = "../assets/Terrain";
    for (const auto& entry : fs::directory_iterator(baseDir)) {
        if (fs::is_directory(entry)) {
            std::string terrainName = entry.path().filename().string();
            terrainNames.push_back(terrainName);
            if (terrainName == terrain->initTerrain) {
                currentTerrainIdx = terrainNames.size() - 1;
            }
        }
    }
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
    // resize the viewport
    this->width = width;
    this->height = height;
    glViewport(0, 0, width, height);

    // resize the framebuffers for post-processing
    fogScreenBuffer->resizeFramebuffer(width, height);
    antiAliasingScreenBuffer->resizeFramebuffer(width, height);
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
    _renderToShadowFramebuffer();
    _renderToWaterFramebuffer();
    _renderToFogFramebuffer();
    _renderToAntiAliasingScreenBuffer();
    _renderToScreen();
}

void Context::_renderToShadowFramebuffer() {
    if (!useShadow)
        return;

    depthMap->bind();
    isRenderingToDepthMap = true;
    glViewport(0, 0, depthMap->width, depthMap->height);
    glClear(GL_DEPTH_BUFFER_BIT);
    terrain->render();
    // water->render();
    depthMap->unbind();
    isRenderingToDepthMap = false;
}

void Context::_renderToWaterFramebuffer() {
    if (!renderWater)
        return;

    glEnable(GL_CLIP_DISTANCE0);
    bool tempShowGround = terrain->showGround;
    terrain->showGround = false;
    // reflection
    water->reflectionBuffer->bind();
    glViewport(0, 0, water->reflectionBuffer->width, water->reflectionBuffer->height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    isRenderingReflection = true;
    // flip camera
    float distance = 2.0f * (camera->position.y - water->waterLevel);
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
    isRenderingReflection = false;
    terrain->render();
    skybox->render();
    water->refractionBuffer->unbind();
    glDisable(GL_CLIP_DISTANCE0);
    terrain->showGround = tempShowGround;
}

void Context::_renderToFogFramebuffer() {
    if (!renderFog)
        return;

    fogScreenBuffer->bind();
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    terrain->render();
    water->render();
    skybox->render();
    fogScreenBuffer->unbind();
}

void Context::_renderToAntiAliasingScreenBuffer() {
    if (!useAntiAliasing)
        return;

    antiAliasingScreenBuffer->bind();
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (renderFog)
        fog->render();
    else {
        terrain->render();
        water->render();
        skybox->render();
    }
    antiAliasingScreenBuffer->unbind();
}

void Context::_renderToScreen() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    bool isPostProcessing = useAntiAliasing || renderFog;
    if (isPostProcessing) {
        if (useAntiAliasing) {
            glDisable(GL_DEPTH_TEST);
            glBindVertexArray(screenQuadVAO);
            FXAAShader->use();
            FXAAShader->bindTexture("screenTexture", antiAliasingScreenBuffer.get());
            FXAAShader->setVec2("u_texelStep", glm::vec2(1.0f / width, 1.0f / height));
            FXAAShader->setFloat("u_lumaThreshold", lumaThreshold);
            FXAAShader->setFloat("u_mulReduce", mulReduce);
            FXAAShader->setFloat("u_minReduce", minReduce);
            FXAAShader->setFloat("u_maxSpan", maxSpan);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glEnable(GL_DEPTH_TEST);
        }
        else if (renderFog) {
            glDisable(GL_DEPTH_TEST);
            fog->render();
            glEnable(GL_DEPTH_TEST);
        }
        return;
    }

    terrain->render();
    water->render();
    skybox->render();
}

glm::vec4 Context::getClipPlane() {
    if (isRenderingReflection)
        return glm::vec4(0.0f, 1.0f, 0.0f, -water->waterLevel);
    else
        return glm::vec4(0.0f, -1.0f, 0.0f, water->waterLevel + 0.25f); // add a small offset to avoid artifacts on border
}

void Context::renderGUI() {
    if (ImGui::Begin("UI Window Example")) {
        if (ImGui::BeginCombo("Terrain Selection", terrainNames[currentTerrainIdx].c_str())) {
            for (int i = 0; i < terrainNames.size(); i++) {
                bool isSelected = (currentTerrainIdx == i);
                if (ImGui::Selectable(terrainNames[i].c_str(), isSelected)) {
                    currentTerrainIdx = i;
                    SPDLOG_INFO("Selected terrain: {}", terrainNames[i]);
                    terrain->resetTerrain(terrainNames[i]);
                }
                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        if (ImGui::TreeNode("Rendering Mode")) {
            if (ImGui::RadioButton("Fill", !wireFrameMode)) {
                wireFrameMode = false;
                renderFog = renderFogSaved;
                useAntiAliasing = useAntiAliasingSaved;
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
            ImGui::SameLine();
            if (ImGui::RadioButton("Wireframe", wireFrameMode)) {
                wireFrameMode = true;
                renderFogSaved = renderFog;
                useAntiAliasingSaved = useAntiAliasing;
                renderFog = false;  // disable post-processing
                useAntiAliasing = false;  // disable post-processing
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            }
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Anti-Aliasing")) {
            ImGui::Checkbox("use anti-aliasing", &useAntiAliasing);
            ImGui::SliderFloat("luma threshold", &lumaThreshold, 0.01f, 0.6f);
            ImGui::SliderFloat("mul reduce", &mulReduce, 1.0 / 16.0, 1.0 / 4.0);
            ImGui::SliderFloat("min reduce", &minReduce, 1.0 / 256.0, 1.0 / 4.0);
            ImGui::SliderFloat("max span", &maxSpan, 4.0f, 16.0f);
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Lighting")) {
            ImGui::Checkbox("show light direction", &showLightDirection);
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
            ImGui::SliderFloat("min shadow bias", &minShadowBias, 0.00001f, maxShadowBias - 0.00001f, "%.5f");
            ImGui::SliderFloat("max shadow bias", &maxShadowBias, minShadowBias + 0.00001f, 0.1f, "%.5f");
            ImGui::SliderInt("num PCF samples", &numPCFSamples, 1, 64);
            ImGui::SliderFloat("PCF spreadness", &PCFSpreadness, 1.0 / 50000.0f, 1.0 / 100.0f, "%.5f");
            ImGui::TreePop();
        }

        if (ImGui::CollapsingHeader("Terrain")) {
            ImGui::Checkbox("render terrain", &renderTerrain);
            ImGui::Checkbox("show ground", &terrain->showGround);
            ImGui::SameLine();
            ImGui::Checkbox("use lighting", &terrain->useLighting);
            ImGui::SameLine();
            ImGui::Checkbox("show normals", &terrain->showNormals);
            ImGui::SliderFloat("height offset", &terrain->heightOffset, -5.0f, 5.0f);
            ImGui::SliderFloat("height scale", &terrain->heightScale, 0.0f, 100.0f);
            ImGui::SliderFloat("horizontal scale", &terrain->horizontalScale, 1.0f, 100.0f);
            ImGui::SliderInt("min tess level", &terrain->minTessLevel, 2, terrain->maxTessLevel - 1);
            ImGui::SliderInt("max tess level", &terrain->maxTessLevel, terrain->minTessLevel + 1, 64);
            ImGui::SliderFloat("min distance", &terrain->minDistance, 1.0f, terrain->maxDistance);
            ImGui::SliderFloat("max distance", &terrain->maxDistance, terrain->minDistance, 100.0f);
            ImGui::SliderFloat("ambient strength", &terrain->ambientStrength, 0.0f, 1.0f);
        }

        if (ImGui::CollapsingHeader("Water")) {
            ImGui::Checkbox("render water", &renderWater);
            ImGui::Checkbox("use DUDV", &water->useDUDV);
            ImGui::Checkbox("specular", &water->specular);
            ImGui::SameLine();
            ImGui::Checkbox("use normal map", &water->useNormalMap);
            ImGui::SliderFloat("water level", &water->waterLevel, 0.0f, 20.0f);
            ImGui::SliderFloat("wave speed", &water->WAVE_SPEED, 0.0f, 0.2f);
            ImGui::SliderFloat("tiling factor", &water->tiling, 1.0f, 10.0f);
        }

        if (ImGui::CollapsingHeader("Fog")) {
            ImGui::Checkbox("render fog", &renderFog);
            ImGui::ColorEdit3("fog color", glm::value_ptr(fog->fogColor));
            ImGui::SliderFloat("fog density", &fog->fogDensity, 0.0f, 10000.0f);
        }
    }
    ImGui::End();

    // ImGui::SetNextWindowCollapsed(true, ImGuiCond_FirstUseEver);
    // if (ImGui::Begin("Depth Map")) {
    //     ImVec2 contentSize = ImGui::GetContentRegionAvail();
    //     int newWidth = (int)contentSize.x;
    //     int newHeight = (int)contentSize.y;

    //     // resize the framebuffer if needed
    //     if (newWidth != sceneBuffer->width || newHeight != sceneBuffer->height) {
    //         newWidth = newWidth > 0 ? newWidth : 1;   // Avoid zero size
    //         newHeight = newHeight > 0 ? newHeight : 1;
    //         sceneBuffer->resizeFramebuffer(newWidth, newHeight);
    //     }

    //     // render the scene to the framebuffer
    //     sceneBuffer->bind();
    //     glViewport(0, 0, sceneBuffer->width, sceneBuffer->height);
    //     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //     depthQuadShader->use();
    //     depthQuadShader->bindTexture("depthMap", depthMap.get());
    //     glBindVertexArray(quadVAO);
    //     glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    //     glBindVertexArray(0);
    //     sceneBuffer->unbind();

    //     // display the framebuffer texture
    //     ImGui::Image(
    //         (ImTextureID)sceneBuffer->texture,
    //         contentSize,
    //         ImVec2(0.0f, 1.0f),
    //         ImVec2(1.0f, 0.0f)
    //     );
    // }
    // ImGui::End();

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
