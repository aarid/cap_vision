#include "../../include/ui/shader.hpp"
#include <iostream>

namespace capvision {
namespace ui {

Shader::Shader() = default;

Shader::~Shader() {
    if (program_) {
        glDeleteProgram(program_);
    }
}

bool Shader::loadFromString(const std::string& vertexShader, 
                          const std::string& fragmentShader) {
    GLuint vertexShaderID = 0, fragmentShaderID = 0;
    
    // Compile shaders
    if (!compileShader(vertexShaderID, GL_VERTEX_SHADER, vertexShader) ||
        !compileShader(fragmentShaderID, GL_FRAGMENT_SHADER, fragmentShader)) {
        return false;
    }

    // Create and link program
    program_ = glCreateProgram();
    glAttachShader(program_, vertexShaderID);
    glAttachShader(program_, fragmentShaderID);
    glLinkProgram(program_);

    // Check link status
    GLint success;
    glGetProgramiv(program_, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(program_, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "Shader program linking failed:\n" << infoLog << std::endl;
        return false;
    }

    // Cleanup
    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);
    
    return true;
}

void Shader::use() {
    glUseProgram(program_);
}

bool Shader::compileShader(GLuint& shader, GLenum type, const std::string& source) {
    shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    // Check compilation status
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "Shader compilation failed:\n" << infoLog << std::endl;
        return false;
    }
    return true;
}

void Shader::setMat4(const std::string& name, const float* value) {
    glUniformMatrix4fv(glGetUniformLocation(program_, name.c_str()), 
                      1, GL_FALSE, value);
}

void Shader::setVec3(const std::string& name, float x, float y, float z) {
    glUniform3f(glGetUniformLocation(program_, name.c_str()), x, y, z);
}

void Shader::setFloat(const std::string& name, float value) {
    glUniform1f(glGetUniformLocation(program_, name.c_str()), value);
}

void Shader::setInt(const std::string& name, int value) {
    glUniform1i(glGetUniformLocation(program_, name.c_str()), value);
}

} // namespace ui
} // namespace capvision