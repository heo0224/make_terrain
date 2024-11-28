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
    heightMap = std::make_unique<Texture>("../assets/Rolling Hills Height Map 1k/converted/Rolling Hills Height Map.png");
    diffuseMap = std::make_unique<Texture>("../assets/Rolling Hills Height Map 1k/converted/Rolling Hills Bitmap 1025.png");

    int width = heightMap->width;
    int height = heightMap->height;
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
    SPDLOG_INFO("Terrain initialized");
}

void Terrain::resetVertices() {
    vertices.clear();
}

void Terrain::render() {
    glm::mat4 model = context->getModelMatrix(
        glm::vec3(0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        0.0f,
        glm::vec3(horizontalScale, 1.0f, horizontalScale)
    );
    glm::mat4 view = context->getViewMatrix();
    glm::mat4 projection = context->getProjectionMatrix();

    shader->use();
    shader->bindTexture("heightMap", heightMap.get(), 0);
    shader->bindTexture("diffuseMap", diffuseMap.get(), 1);
    shader->setMat4("model", model);
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);
    shader->setFloat("heightScale", heightScale);
    shader->setFloat("heightOffset", heightOffset);
    shader->setInt("minTessLevel", minTessLevel);
    shader->setInt("maxTessLevel", maxTessLevel);
    shader->setFloat("minDistance", minDistance);
    shader->setFloat("maxDistance", maxDistance);
    glBindVertexArray(VAO);
    glDrawArrays(GL_PATCHES, 0, 4 * numStrips * numStrips);
}