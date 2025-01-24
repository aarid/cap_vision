#pragma once

#include <string>
#include <GL/glew.h>

namespace capvision {
namespace ui {

class Shader {
public:
    Shader();
    ~Shader();

    bool loadFromString(const std::string& vertexShader, 
                       const std::string& fragmentShader);
    void use();
    GLuint getProgram() const { return program_; }

    // Utility functions for setting uniforms
    void setMat4(const std::string& name, const float* value);
    void setVec3(const std::string& name, float x, float y, float z);
    void setFloat(const std::string& name, float value);
    void setInt(const std::string& name, int value);

private:
    GLuint program_{0};
    bool compileShader(GLuint& shader, GLenum type, const std::string& source);
};

} // namespace ui
} // namespace capvision