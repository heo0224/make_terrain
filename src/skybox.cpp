#include "common.h"
#include "skybox.h"
#include "utils.h"
#include "context.h"
#include "geometry_primitives.h"

Skybox::Skybox(Context* context) : context(context) {
    shader = std::make_unique<Shader>("../shaders/shader_skybox.vs", "../shaders/shader_skybox.fs");
    texture = std::make_unique<CubemapTexture>(
        std::vector<std::string> {
        "../assets/skybox/right.tga",
            "../assets/skybox/left.tga",
            "../assets/skybox/top.tga",
            "../assets/skybox/bottom.tga",
            "../assets/skybox/front.tga",
            "../assets/skybox/back.tga"
    }
    );
    VAO = generatePositionVAO(skyBoxPositions, sizeof(skyBoxPositions));
}

void Skybox::render() {
    glDepthFunc(GL_LEQUAL);
    glBindVertexArray(VAO);
    glm::mat4 view = glm::mat4(glm::mat3(context->getViewMatrix()));
    glm::mat4 projection = context->getProjectionMatrix();
    shader->use();
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);
    shader->bindCubemapTexture("skyboxTexture1", texture.get());
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthFunc(GL_LESS);
}