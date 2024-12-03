#include "shader.h"
#include <fstream>
#include <sstream>

Shader::Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath, const char* tcsPath, const char* tesPath)
{
    // Create the shader program
    ID = glCreateProgram();

    // Load and compile shaders
    unsigned int vertexShader = loadShader(vertexPath, GL_VERTEX_SHADER);
    unsigned int fragmentShader = loadShader(fragmentPath, GL_FRAGMENT_SHADER);

    unsigned int geometryShader = 0;
    if (geometryPath != nullptr)
        geometryShader = loadShader(geometryPath, GL_GEOMETRY_SHADER);

    unsigned int tcsShader = 0;
    if (tcsPath != nullptr)
        tcsShader = loadShader(tcsPath, GL_TESS_CONTROL_SHADER);

    unsigned int tesShader = 0;
    if (tesPath != nullptr)
        tesShader = loadShader(tesPath, GL_TESS_EVALUATION_SHADER);

    // Link the program
    glLinkProgram(ID);
    // Check for linking errors
    checkCompileErrors(ID, "PROGRAM");

    // Delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    if (geometryPath != nullptr)
        glDeleteShader(geometryShader);
    if (tcsPath != nullptr)
        glDeleteShader(tcsShader);
    if (tesPath != nullptr)
        glDeleteShader(tesShader);
}

void Shader::use()
{
    glUseProgram(ID);
}

void Shader::bindTexture(const std::string& name, const Texture* texture, int unit)
{
    if (unit >= MAX_TEXTURE_UNITS) {
        SPDLOG_ERROR("Texture unit is out of range: {}", unit);
        return;
    }

    if (bindedTextureNames[unit] != name) {
        GLint location = glGetUniformLocation(ID, name.c_str());
        if (location == -1) {
            SPDLOG_ERROR("Uniform not found: {}", name);
            return;
        }
        glUniform1i(location, unit);
        bindedTextureNames[unit] = name;
    }
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, texture->ID);
}

void Shader::bindTexture(const std::string& name, const Framebuffer* framebuffer, int unit)
{
    if (unit >= MAX_TEXTURE_UNITS) {
        SPDLOG_ERROR("Texture unit is out of range: {}", unit);
        return;
    }

    if (bindedTextureNames[unit] != name) {
        GLint location = glGetUniformLocation(ID, name.c_str());
        if (location == -1) {
            SPDLOG_ERROR("Uniform not found: {}", name);
            return;
        }
        glUniform1i(location, unit);
        bindedTextureNames[unit] = name;
    }
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, framebuffer->texture);
}

void Shader::bindTexture(const std::string& name, unsigned int textureID, int unit)
{
    if (unit >= MAX_TEXTURE_UNITS) {
        SPDLOG_ERROR("Texture unit is out of range: {}", unit);
        return;
    }

    if (bindedTextureNames[unit] != name) {
        GLint location = glGetUniformLocation(ID, name.c_str());
        if (location == -1) {
            SPDLOG_ERROR("Uniform not found: {}", name);
            return;
        }
        glUniform1i(location, unit);
        bindedTextureNames[unit] = name;
    }
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, textureID);
}

void Shader::bindCubemapTexture(const std::string& name, const CubemapTexture* texture, int unit)
{
    if (unit >= MAX_TEXTURE_UNITS) {
        SPDLOG_ERROR("Texture unit is out of range: {}", unit);
        return;
    }

    if (bindedTextureNames[unit] != name) {
        GLint location = glGetUniformLocation(ID, name.c_str());
        if (location == -1) {
            SPDLOG_ERROR("Uniform not found: {}", name);
            return;
        }
        glUniform1i(location, unit);
        bindedTextureNames[unit] = name;
    }
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture->textureID);
}

// Utility uniform functions
void Shader::setBool(const std::string& name, bool value) const
{
    GLint location = glGetUniformLocation(ID, name.c_str());
    if (location == -1) {
        SPDLOG_ERROR("Uniform not found: {}", name);
        return;
    }
    glUniform1i(location, static_cast<int>(value));
}

void Shader::setInt(const std::string& name, int value) const
{
    GLint location = glGetUniformLocation(ID, name.c_str());
    if (location == -1) {
        SPDLOG_ERROR("Uniform not found: {}", name);
        return;
    }
    glUniform1i(location, value);
}

void Shader::setFloat(const std::string& name, float value) const
{
    GLint location = glGetUniformLocation(ID, name.c_str());
    if (location == -1) {
        SPDLOG_ERROR("Uniform not found: {}", name);
        return;
    }
    glUniform1f(location, value);
}

void Shader::setVec2(const std::string& name, const glm::vec2& value) const
{
    GLint location = glGetUniformLocation(ID, name.c_str());
    if (location == -1) {
        SPDLOG_ERROR("Uniform not found: {}", name);
        return;
    }
    glUniform2fv(location, 1, &value[0]);
}

void Shader::setVec2(const std::string& name, float x, float y) const
{
    GLint location = glGetUniformLocation(ID, name.c_str());
    if (location == -1) {
        SPDLOG_ERROR("Uniform not found: {}", name);
        return;
    }
    glUniform2f(location, x, y);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
    GLint location = glGetUniformLocation(ID, name.c_str());
    if (location == -1) {
        SPDLOG_ERROR("Uniform not found: {}", name);
        return;
    }
    glUniform3fv(location, 1, &value[0]);
}

void Shader::setVec3(const std::string& name, float x, float y, float z) const
{
    GLint location = glGetUniformLocation(ID, name.c_str());
    if (location == -1) {
        SPDLOG_ERROR("Uniform not found: {}", name);
        return;
    }
    glUniform3f(location, x, y, z);
}

void Shader::setVec4(const std::string& name, const glm::vec4& value) const
{
    GLint location = glGetUniformLocation(ID, name.c_str());
    if (location == -1) {
        SPDLOG_ERROR("Uniform not found: {}", name);
        return;
    }
    glUniform4fv(location, 1, &value[0]);
}

void Shader::setVec4(const std::string& name, float x, float y, float z, float w)
{
    GLint location = glGetUniformLocation(ID, name.c_str());
    if (location == -1) {
        SPDLOG_ERROR("Uniform not found: {}", name);
        return;
    }
    glUniform4f(location, x, y, z, w);
}

void Shader::setMat2(const std::string& name, const glm::mat2& mat) const
{
    GLint location = glGetUniformLocation(ID, name.c_str());
    if (location == -1) {
        SPDLOG_ERROR("Uniform not found: {}", name);
        return;
    }
    glUniformMatrix2fv(location, 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat3(const std::string& name, const glm::mat3& mat) const
{
    GLint location = glGetUniformLocation(ID, name.c_str());
    if (location == -1) {
        SPDLOG_ERROR("Uniform not found: {}", name);
        return;
    }
    glUniformMatrix3fv(location, 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat) const
{
    GLint location = glGetUniformLocation(ID, name.c_str());
    if (location == -1) {
        SPDLOG_ERROR("Uniform not found: {}", name);
        return;
    }
    glUniformMatrix4fv(location, 1, GL_FALSE, &mat[0][0]);
}

// Utility function for checking shader compilation/linking errors
void Shader::checkCompileErrors(GLuint shader, std::string type)
{
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            SPDLOG_ERROR("ERROR::SHADER_COMPILATION_ERROR of type: {}", type);
            SPDLOG_ERROR("{}", infoLog);
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            SPDLOG_ERROR("ERROR::PROGRAM_LINKING_ERROR of type: {}", type);
            SPDLOG_ERROR("{}", infoLog);
        }
    }
}

// Loads and compiles individual shaders
unsigned int Shader::loadShader(std::string path, unsigned int shaderType)
{
    std::string code;
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // Open the file
        file.open(path);
        std::stringstream shaderStream;
        // Read file's buffer contents into streams
        shaderStream << file.rdbuf();
        // Close file handlers
        file.close();
        // Convert stream into string
        code = shaderStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        SPDLOG_ERROR("ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: {}", e.what());
    }

    const char* shaderCode = code.c_str();

    // Create shader object
    unsigned int shaderID = glCreateShader(shaderType);
    glShaderSource(shaderID, 1, &shaderCode, NULL);
    glCompileShader(shaderID);

    // Determine shader type for error checking
    std::string type;
    switch (shaderType)
    {
        case GL_VERTEX_SHADER:
            type = "VERTEX";
            break;
        case GL_FRAGMENT_SHADER:
            type = "FRAGMENT";
            break;
        case GL_GEOMETRY_SHADER:
            type = "GEOMETRY";
            break;
        case GL_TESS_CONTROL_SHADER:
            type = "TESS_CONTROL";
            break;
        case GL_TESS_EVALUATION_SHADER:
            type = "TESS_EVALUATION";
            break;
        default:
            type = "UNKNOWN";
    }

    // Check for compilation errors
    checkCompileErrors(shaderID, type);

    // Attach shader to program
    glAttachShader(ID, shaderID);

    return shaderID;
}

   