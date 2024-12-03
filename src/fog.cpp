#include "fog.h"
#include "geometry_primitives.h"
#include "utils.h"
#include "context.h"

Fog::Fog(Context* context) : context(context) {
    init();
}

void Fog::init() {
    fogShader = std::make_unique<Shader>(
        "../shaders/shader_fog.vs",
        "../shaders/shader_fog.fs"
    );
    screenQuadVAO = generatePositionTextureVAO(screenQuadVertices, sizeof(screenQuadVertices));
}

void Fog::render() {
    fogShader->use();
    glBindVertexArray(screenQuadVAO);
    fogShader->bindTexture("sceneBuffer", context->sceneDepthBuffer->colorTexture, 0);
    fogShader->bindTexture("depthMap", context->sceneDepthBuffer->depthTexture, 1);
    // fogShader->bindTexture("depthMap", context->depthMap.get(), 1);

    fogShader->setVec3("fogColor", fogColor);
    fogShader->setFloat("fogDensity", fogDensity);
    fogShader->setFloat("nearPlane", 0.1f);
    fogShader->setFloat("farPlane", 100000.0f);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}