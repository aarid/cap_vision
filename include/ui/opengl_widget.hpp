#pragma once

#include <GL/glew.h>
#include <QtOpenGLWidgets/QOpenGLWidget>
#include <QtGui/QOpenGLFunctions>
#include <opencv2/opencv.hpp>
#include "../../include/core/face_detector.hpp"
#include "../../include/ui/shader.hpp"

namespace capvision {
namespace ui {

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

public:
    explicit OpenGLWidget(QWidget* parent = nullptr);
    ~OpenGLWidget();

    // Update frame and face detection results
    void updateFrame(const cv::Mat& frame, 
                    const core::FaceDetector::FaceDetectionResult& face);

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

private:
    // Current frame and face detection results
    cv::Mat currentFrame_;
    core::FaceDetector::FaceDetectionResult faceResult_;
    bool hasNewFrame_{false};

    // OpenGL stuff
    GLuint textureId_{0};
    void updateTexture();

    ui::Shader shader_;
    
    // Shaders source
    const std::string vertexShaderSource_ = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec2 aTexCoord;
        
        out vec2 TexCoord;
        
        void main() {
            gl_Position = vec4(aPos, 1.0);
            TexCoord = aTexCoord;
        }
    )";

    const std::string fragmentShaderSource_ = R"(
        #version 330 core
        out vec4 FragColor;
        
        in vec2 TexCoord;
        uniform sampler2D videoTexture;
        
        void main() {
            vec4 color = texture(videoTexture, TexCoord);
            // Ajout d'un effet simple, par exemple une teinte bleue
            FragColor = vec4(color.r * 0.2, color.g * 0.2, color.b * 1.2, 1.0);
        }
    )";

     // Vertex buffer objects
    GLuint VBO_{0};
    GLuint VAO_{0};
    GLuint EBO_{0};

    // Setup functions
    void setupQuad();
};

} // namespace ui
} // namespace capvision