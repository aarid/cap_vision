#include "../../include/ui/opengl_widget.hpp"

namespace capvision {
namespace ui {

OpenGLWidget::OpenGLWidget(QWidget* parent)
    : QOpenGLWidget(parent)
{
}

OpenGLWidget::~OpenGLWidget() {
    makeCurrent();
    if (textureId_) glDeleteTextures(1, &textureId_);
    if (quadVAO_) glDeleteVertexArrays(1, &quadVAO_);
    if (quadVBO_) glDeleteBuffers(1, &quadVBO_);
    if (quadEBO_) glDeleteBuffers(1, &quadEBO_);
    doneCurrent();
}

void OpenGLWidget::initializeGL() {
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return;
    }

    initializeOpenGLFunctions();
    
    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Setup shaders
    setupShaders();

    // Setup quad for video rendering
    setupQuad();

    // Initialize texture
    glGenTextures(1, &textureId_);
    glBindTexture(GL_TEXTURE_2D, textureId_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void OpenGLWidget::setupShaders() {
    if (!videoShader_.loadFromString(videoVertexShaderSource_, videoFragmentShaderSource_)) {
        std::cerr << "Failed to load video shaders" << std::endl;
        return;
    }

    if (!modelShader_.loadFromString(modelVertexShaderSource_, modelFragmentShaderSource_)) {
        std::cerr << "Failed to load model shaders" << std::endl;
        return;
    }
}

void OpenGLWidget::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
    aspectRatio_ = static_cast<float>(w) / static_cast<float>(h);
    
    // Update projection matrix
    projection_ = glm::perspective(glm::radians(45.0f), aspectRatio_, 0.1f, 100.0f);
}

void OpenGLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render video background
    renderVideo();

    // Render 3D model (will be implemented)
    renderModel();
}

void OpenGLWidget::renderVideo() {
    if (hasNewFrame_) {
        updateTexture();
        hasNewFrame_ = false;
    }

    glDisable(GL_DEPTH_TEST);  // Disable depth testing for video
    
    videoShader_.use();
    videoShader_.setInt("videoTexture", 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId_);

    glBindVertexArray(quadVAO_);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);  // Re-enable depth testing for 3D
}

void OpenGLWidget::renderModel() {
    // Will be implemented in the next step
}


void OpenGLWidget::setupQuad() {
    float vertices[] = {
        // positions        // texture coords
        -1.0f,  1.0f, 0.0f,  0.0f, 0.0f,  // top left
         1.0f,  1.0f, 0.0f,  1.0f, 0.0f,  // top right
         1.0f, -1.0f, 0.0f,  1.0f, 1.0f,  // bottom right
        -1.0f, -1.0f, 0.0f,  0.0f, 1.0f   // bottom left
    };

    unsigned int indices[] = {
        0, 1, 2,  // first triangle
        0, 2, 3   // second triangle
    };

    glGenVertexArrays(1, &quadVAO_);
    glGenBuffers(1, &quadVBO_);
    glGenBuffers(1, &quadEBO_);

    glBindVertexArray(quadVAO_);

    glBindBuffer(GL_ARRAY_BUFFER, quadVBO_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Texture coordinate attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void OpenGLWidget::updateTexture() {
    if (currentFrame_.empty()) return;

    glBindTexture(GL_TEXTURE_2D, textureId_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 currentFrame_.cols, currentFrame_.rows, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, currentFrame_.data);
}


void OpenGLWidget::updateFrame(const cv::Mat& frame,
                             const core::FaceDetector::FaceDetectionResult& face)
{
    if (frame.empty()) return;

    cv::Mat temp;
    cv::cvtColor(frame, temp, cv::COLOR_BGR2RGB);
    currentFrame_ = temp.clone();
    faceResult_ = face;
    hasNewFrame_ = true;
    update(); // Trigger repaint
}

} // namespace ui
} // namespace capvision