#include "water.h"
#include "geometry_primitives.h"
#include "utils.h"
#include "context.h"

Water::Water(Context* context) : context(context) {
    init();
}

void Water::init(){
    this->reflectionBuffer = Framebuffer::create(this->width, this->height, AttachmentType::COLOR);
    this->refractionBuffer = Framebuffer::create(this->width, this->height, AttachmentType::COLOR);
    waterShader = std::make_unique<Shader>(
        "../shaders/shader_water.vs",
        "../shaders/shader_water.fs"
    );
    dudvMap = std::make_unique<Texture>("../assets/Water/dudv.png");
    normalMap = std::make_unique<Texture>("../assets/Water/normal.png");
    waterVAO = generatePositionTextureVAOWithEBO(quadPositionTextures, sizeof(quadPositionTextures), quadIndices, sizeof(quadIndices));
}

void Water::render(){
    waterShader->use();
    waterShader->bindTexture("reflectionTexture", reflectionBuffer.get(),0);
    waterShader->bindTexture("refractionTexture", refractionBuffer.get(),1);
    waterShader->bindTexture("dudvMap", dudvMap.get(),2);
    waterVAO = generatePositionTextureVAOWithEBO(quadPositionTextures, sizeof(quadPositionTextures), quadIndices, sizeof(quadIndices));
    glBindVertexArray(waterVAO);
    waterShader->setMat4("projection", context->getProjectionMatrix());
    waterShader->setMat4("view", context->getViewMatrix());
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(waterSize, 1.0f, waterSize));
    model = glm::translate(model, glm::vec3(0.0f, waterLevel, 0.0f));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    waterShader->setMat4("model", model);
    waterShader->setFloat("mixfactor", context->mixFactor);
    waterShader->setBool("useDUDV", context->useDUDV);
    float moveFactor = WAVE_SPEED * glfwGetTime();
    moveFactor = fmod(moveFactor, 1.0f);
    waterShader->setFloat("moveFactor", moveFactor);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}