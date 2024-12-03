#ifndef __SHADER_H__
#define __SHADER_H__

#include "common.h"
#include "texture.h"
#include "framebuffer.h"
#include <string>

constexpr int MAX_TEXTURE_UNITS = 8;

class Shader
{
public:
    unsigned int ID;
    Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr,
        const char* tcsPath = nullptr, const char* tesPath = nullptr);

    void use();
    void bindTexture(const std::string& name, const Texture* texture, int unit = 0);
    void bindTexture(const std::string& name, const Framebuffer* framebuffer, int unit = 0);
    void bindTexture(const std::string& anme, unsigned int textureID, int unit = 0);
    void bindCubemapTexture(const std::string& name, const CubemapTexture* texture, int unit = 0);
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec2(const std::string& name, const glm::vec2& value) const;
    void setVec2(const std::string& name, float x, float y) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec3(const std::string& name, float x, float y, float z) const;
    void setVec4(const std::string& name, const glm::vec4& value) const;
    void setVec4(const std::string& name, float x, float y, float z, float w);
    void setMat2(const std::string& name, const glm::mat2& mat) const;
    void setMat3(const std::string& name, const glm::mat3& mat) const;
    void setMat4(const std::string& name, const glm::mat4& mat) const;
private:
    void checkCompileErrors(GLuint shader, std::string type);
    unsigned int loadShader(std::string path, unsigned int shaderType);

    std::string bindedTextureNames[MAX_TEXTURE_UNITS] = { "" };
};


#endif  // __SHADER_H__