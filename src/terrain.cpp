#include "common.h"
#include "utils.h"
#include "terrain.h"
#include "context.h"
#include <stb/stb_image.h>


std::unique_ptr<Terrain> Terrain::createWithTessellation(Context* context) {
    auto terrain = std::unique_ptr<Terrain>(new Terrain(context));
    terrain->init();
    return std::move(terrain);
}

std::unique_ptr<Terrain> Terrain::createWithoutTessellation(Context* context) {
    SPDLOG_ERROR("Terrain without tessellation is not implemented yet");
    auto terrain = std::unique_ptr<Terrain>(new Terrain(context));
    return std::move(terrain);
}

void Terrain::init() {
    shader = std::make_unique<Shader>(
        "../shaders/terrain/shader_terrain.vs",
        "../shaders/terrain/shader_terrain.fs",
        "../shaders/terrain/shader_terrain.gs",
        "../shaders/terrain/shader_terrain.tesc",
        "../shaders/terrain/shader_terrain.tese"
    );
    normalShader = std::make_unique<Shader>(
        "../shaders/terrain/shader_terrain.vs",
        "../shaders/debug/shader_terrain_normal.fs",
        "../shaders/debug/shader_terrain_normal.gs",
        "../shaders/terrain/shader_terrain.tesc",
        "../shaders/terrain/shader_terrain.tese"
    );

    resetTerrain(initTerrain);
    SPDLOG_INFO("Terrain initialized");
}

void Terrain::resetTerrain(const std::string& terrainName) {
    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
    }

    heightMap = std::make_unique<Texture>(("../assets/Terrain/" + terrainName + "/converted/Height Map.png").c_str());
    diffuseMap = std::make_unique<Texture>(("../assets/Terrain/" + terrainName + "/converted/Diffuse Map.png").c_str());

    int width = heightMap->width;
    int height = heightMap->height;
    numStrips = width / 50;

    vertices.clear();
    for (unsigned i = 0; i < numStrips; i++)
    {
        for (unsigned j = 0; j < numStrips; j++)
        {
            // bottom-left point of a quad
            vertices.push_back(-width / 2.0f + width * i / (float)numStrips); // v.x
            vertices.push_back(0.0f); // v.y
            vertices.push_back(-height / 2.0f + height * j / (float)numStrips); // v.z
            vertices.push_back(i / (float)numStrips); // u
            vertices.push_back(j / (float)numStrips); // v

            // bottom-right point of a quad
            vertices.push_back(-width / 2.0f + width * (i + 1) / (float)numStrips); // v.x
            vertices.push_back(0.0f); // v.y
            vertices.push_back(-height / 2.0f + height * j / (float)numStrips); // v.z
            vertices.push_back((i + 1) / (float)numStrips); // u
            vertices.push_back(j / (float)numStrips); // v

            // top-left point of a quad
            vertices.push_back(-width / 2.0f + width * i / (float)numStrips); // v.x
            vertices.push_back(0.0f); // v.y
            vertices.push_back(-height / 2.0f + height * (j + 1) / (float)numStrips); // v.z
            vertices.push_back(i / (float)numStrips); // u
            vertices.push_back((j + 1) / (float)numStrips); // v

            // top-right point of a quad
            vertices.push_back(-width / 2.0f + width * (i + 1) / (float)numStrips); // v.x
            vertices.push_back(0.0f); // v.y
            vertices.push_back(-height / 2.0f + height * (j + 1) / (float)numStrips); // v.z
            vertices.push_back((i + 1) / (float)numStrips); // u
            vertices.push_back((j + 1) / (float)numStrips); // v
        }
    }

    VAO = generatePositionTextureVAO(vertices);
    SPDLOG_INFO("Terrain reset: {}", terrainName);
    SPDLOG_INFO("Terrain width: {}, height: {}, numStrips: {}", width, height, numStrips);
}


void Terrain::render() {
    if (!context->renderTerrain)
        return;

    glm::mat4 model = context->getModelMatrix(
        glm::vec3(0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        0.0f,
        glm::vec3(horizontalScale / (float)heightMap->width, 1.0f, horizontalScale / (float)heightMap->height)
    );
    glm::mat4 view = context->getViewMatrix();
    glm::mat4 projection = context->getProjectionMatrix();

    shader->use();
    shader->setMat4("model", model);
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);

    // terrain
    shader->bindTexture("heightMap", heightMap.get(), 0);
    shader->bindTexture("diffuseMap", diffuseMap.get(), 1);
    shader->setFloat("heightScale", heightScale);
    shader->setFloat("heightOffset", heightOffset);
    shader->setInt("minTessLevel", minTessLevel);
    shader->setInt("maxTessLevel", maxTessLevel);
    shader->setFloat("minDistance", minDistance);
    shader->setFloat("maxDistance", maxDistance);
    shader->setBool("showGround", showGround);

    // light
    shader->setBool("useLighting", useLighting);
    shader->setFloat("ambientStrength", ambientStrength);
    shader->setVec3("lightDir", context->light->direction);
    shader->setMat4("lightSpaceMatrix", context->light->getLightSpaceMatrix());

    // shadow
    shader->bindTexture("depthMap", context->depthMap.get(), 2);
    shader->setBool("renderToDepthMap", context->isRenderingToDepthMap);
    shader->setBool("useShadow", context->useShadow);
    shader->setBool("usePCF", context->usePCF);
    shader->setFloat("minShadowBias", context->minShadowBias);
    shader->setFloat("maxShadowBias", context->maxShadowBias);
    shader->setInt("numPCFSamples", context->numPCFSamples);
    shader->setFloat("PCFSpreadness", context->PCFSpreadness);

    // clip plane
    shader->setVec4("clipPlane", context->getClipPlane());

    glBindVertexArray(VAO);
    glDrawArrays(GL_PATCHES, 0, 4 * numStrips * numStrips);

    // debug: show normals or light direction
    if (showNormals || context->showLightDirection) {
        normalShader->use();
        normalShader->setMat4("model", model);
        normalShader->setMat4("view", view);
        normalShader->setMat4("projection", projection);
        normalShader->bindTexture("heightMap", heightMap.get(), 0);
        normalShader->setFloat("heightScale", heightScale);
        normalShader->setFloat("heightOffset", heightOffset);
        normalShader->setInt("minTessLevel", minTessLevel);
        normalShader->setInt("maxTessLevel", maxTessLevel);
        normalShader->setFloat("minDistance", minDistance);
        normalShader->setFloat("maxDistance", maxDistance);
        normalShader->setBool("showNormals", showNormals);
        normalShader->setBool("showLightDirection", context->showLightDirection);
        normalShader->setVec3("lightDir", context->light->direction);
        glBindVertexArray(VAO);
        glDrawArrays(GL_PATCHES, 0, 4 * numStrips * numStrips);
    }
}