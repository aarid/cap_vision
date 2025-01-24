#pragma once

#include <GL/glew.h>
#include <QtOpenGLWidgets/QOpenGLWidget>
#include <QtGui/QOpenGLFunctions>
#include <opencv2/opencv.hpp>
#include "../../include/core/face_detector.hpp"
#include "../../include/ui/shader.hpp"
#include "../../include/ui/model3d.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace capvision {
namespace ui {

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

public:
    explicit OpenGLWidget(QWidget* parent = nullptr);
    ~OpenGLWidget();

    void updateFrame(const cv::Mat& frame, 
                    const core::FaceDetector::FaceDetectionResult& face);

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

private:
    cv::Mat currentFrame_;
    core::FaceDetector::FaceDetectionResult faceResult_;
    bool hasNewFrame_{false};
    GLuint textureId_{0};
    Shader videoShader_;  // Renamed from shader_
    GLuint quadVAO_{0}, quadVBO_{0}, quadEBO_{0};

    Shader modelShader_;
    glm::mat4 projection_{1.0f};
    glm::mat4 view_{1.0f};
    float aspectRatio_{1.0f};

    std::unique_ptr<Model3D> model_;
    glm::mat4 modelMatrix_{1.0f};
    

    // for models transformations
    float modelScale_{1.0f};
    glm::vec3 modelPosition_{0.0f, 0.0f, 0.0f};
    glm::vec3 modelRotation_{0.0f, 0.0f, 0.0f};

    // Setup functions
    void setupQuad();
    void setupShaders();
    void updateTexture();
    void renderVideo();
    void renderModel();  // Will be implemented later

    // Shader sources
    const std::string videoVertexShaderSource_ = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec2 aTexCoord;
        
        out vec2 TexCoord;
        
        void main() {
            gl_Position = vec4(aPos, 1.0);
            TexCoord = aTexCoord;
        }
    )";

    const std::string videoFragmentShaderSource_ = R"(
        #version 330 core
        out vec4 FragColor;
        
        in vec2 TexCoord;
        uniform sampler2D videoTexture;
        
        void main() {
            FragColor = texture(videoTexture, TexCoord);
        }
    )";

    // New shaders for 3D rendering
    const std::string modelVertexShaderSource_ = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aNormal;
        layout (location = 2) in vec2 aTexCoord;

        out vec2 TexCoord;
        out vec3 Normal;
        out vec3 FragPos;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

        void main() {
            FragPos = vec3(model * vec4(aPos, 1.0));
            Normal = mat3(transpose(inverse(model))) * aNormal;
            TexCoord = aTexCoord;
            gl_Position = projection * view * model * vec4(aPos, 1.0);
        }
    )";

    const std::string modelFragmentShaderSource_ = R"(
        #version 330 core
        out vec4 FragColor;

        in vec2 TexCoord;
        in vec3 Normal;
        in vec3 FragPos;

        uniform sampler2D texture_diffuse1;
        uniform vec3 lightPos;
        uniform vec3 viewPos;

        void main() {
            // Basic lighting
            vec3 norm = normalize(Normal);
            vec3 lightDir = normalize(lightPos - FragPos);
            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuse = diff * vec3(1.0, 1.0, 1.0);

            // Ambient
            vec3 ambient = vec3(0.2, 0.2, 0.2);

            // Final color
            vec4 texColor = texture(texture_diffuse1, TexCoord);
            vec3 result = (ambient + diffuse) * texColor.rgb;
            FragColor = vec4(result, texColor.a);
        }
    )";
};

} // namespace ui
} // namespace capvision