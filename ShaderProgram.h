#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <string>
#include <gl/glew/glew.h>
#include "Vector3.h"

/**
 * @class ShaderProgram
 * @brief Manages OpenGL shader programs (compilation, linking, and uniform setting)
 * 
 * Single Responsibility: Handles all shader-related operations
 * Open/Closed: Can be extended without modifying existing code
 */
class ShaderProgram {
public:
    ShaderProgram();
    ~ShaderProgram();

    // Disable copy operations to prevent resource issues
    ShaderProgram(const ShaderProgram&) = delete;
    ShaderProgram& operator=(const ShaderProgram&) = delete;

    // Enable move operations for efficient resource management
    ShaderProgram(ShaderProgram&& other) noexcept;
    ShaderProgram& operator=(ShaderProgram&& other) noexcept;

    /**
     * @brief Load and compile shaders from files
     * @param vertexPath Path to vertex shader file
     * @param fragmentPath Path to fragment shader file
     * @return true if successful, false otherwise
     */
    bool loadFromFile(const std::string& vertexPath, const std::string& fragmentPath);

    /**
     * @brief Activate this shader program for rendering
     */
    void use() const;

    /**
     * @brief Get the OpenGL program ID
     */
    GLuint getProgram() const { return m_program; }

    // Uniform setters
    void setUniform(const std::string& name, int value) const;
    void setUniform(const std::string& name, float value) const;
    void setUniform(const std::string& name, const Vector3& value) const;
    void setUniformMatrix4fv(const std::string& name, const float* matrix) const;

private:
    GLuint m_program;
    
    /**
     * @brief Compile a shader from source code
     */
    GLuint compileShader(const std::string& source, GLenum type);
    
    /**
     * @brief Link vertex and fragment shaders into a program
     */
    bool linkProgram(GLuint vertexShader, GLuint fragmentShader);
    
    /**
     * @brief Check for shader compilation errors
     */
    bool checkCompileErrors(GLuint shader, const std::string& type);
    
    /**
     * @brief Check for program linking errors
     */
    bool checkLinkErrors();
    
    /**
     * @brief Read shader source from file
     */
    std::string readShaderFile(const std::string& filepath);
};

#endif // SHADERPROGRAM_H