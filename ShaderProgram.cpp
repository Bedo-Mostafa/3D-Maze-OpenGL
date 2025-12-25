#include "ShaderProgram.h"
#include <fstream>
#include <sstream>
#include <iostream>

ShaderProgram::ShaderProgram() : m_program(0) {}

ShaderProgram::~ShaderProgram() {
    if (m_program != 0) {
        glDeleteProgram(m_program);
    }
}

ShaderProgram::ShaderProgram(ShaderProgram&& other) noexcept : m_program(other.m_program) {
    other.m_program = 0;
}

ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other) noexcept {
    if (this != &other) {
        if (m_program != 0) {
            glDeleteProgram(m_program);
        }
        m_program = other.m_program;
        other.m_program = 0;
    }
    return *this;
}

bool ShaderProgram::loadFromFile(const std::string& vertexPath, const std::string& fragmentPath) {
    // Read shader sources
    std::string vertexSource = readShaderFile(vertexPath);
    std::string fragmentSource = readShaderFile(fragmentPath);

    if (vertexSource.empty() || fragmentSource.empty()) {
        std::cerr << "Failed to read shader files" << std::endl;
        return false;
    }

    // Compile shaders
    GLuint vertexShader = compileShader(vertexSource, GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader(fragmentSource, GL_FRAGMENT_SHADER);

    if (vertexShader == 0 || fragmentShader == 0) {
        if (vertexShader != 0) glDeleteShader(vertexShader);
        if (fragmentShader != 0) glDeleteShader(fragmentShader);
        return false;
    }

    // Link program
    bool success = linkProgram(vertexShader, fragmentShader);

    // Clean up shaders (no longer needed after linking)
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return success;
}

void ShaderProgram::use() const {
    glUseProgram(m_program);
}

GLuint ShaderProgram::compileShader(const std::string& source, GLenum type) {
    GLuint shader = glCreateShader(type);
    const char* sourceCStr = source.c_str();
    glShaderSource(shader, 1, &sourceCStr, nullptr);
    glCompileShader(shader);

    std::string typeStr = (type == GL_VERTEX_SHADER) ? "VERTEX" : "FRAGMENT";
    if (!checkCompileErrors(shader, typeStr)) {
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

bool ShaderProgram::linkProgram(GLuint vertexShader, GLuint fragmentShader) {
    m_program = glCreateProgram();
    glAttachShader(m_program, vertexShader);
    glAttachShader(m_program, fragmentShader);
    glLinkProgram(m_program);

    return checkLinkErrors();
}

bool ShaderProgram::checkCompileErrors(GLuint shader, const std::string& type) {
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    
    if (!success) {
        GLchar infoLog[1024];
        glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
        std::cerr << "Shader compilation error (" << type << "):\n" << infoLog << std::endl;
        return false;
    }
    return true;
}

bool ShaderProgram::checkLinkErrors() {
    GLint success;
    glGetProgramiv(m_program, GL_LINK_STATUS, &success);
    
    if (!success) {
        GLchar infoLog[1024];
        glGetProgramInfoLog(m_program, 1024, nullptr, infoLog);
        std::cerr << "Shader linking error:\n" << infoLog << std::endl;
        return false;
    }
    return true;
}

std::string ShaderProgram::readShaderFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open shader file: " << filepath << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void ShaderProgram::setUniform(const std::string& name, int value) const {
    GLint location = glGetUniformLocation(m_program, name.c_str());
    if (location != -1) {
        glUniform1i(location, value);
    }
}

void ShaderProgram::setUniform(const std::string& name, float value) const {
    GLint location = glGetUniformLocation(m_program, name.c_str());
    if (location != -1) {
        glUniform1f(location, value);
    }
}

void ShaderProgram::setUniform(const std::string& name, const Vector3& value) const {
    GLint location = glGetUniformLocation(m_program, name.c_str());
    if (location != -1) {
        glUniform3f(location, value.x, value.y, value.z);
    }
}

void ShaderProgram::setUniformMatrix4fv(const std::string& name, const float* matrix) const {
    GLint location = glGetUniformLocation(m_program, name.c_str());
    if (location != -1) {
        glUniformMatrix4fv(location, 1, GL_FALSE, matrix);
    }
}