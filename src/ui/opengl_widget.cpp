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
    if (VAO_) glDeleteVertexArrays(1, &VAO_);
    if (VBO_) glDeleteBuffers(1, &VBO_);
    if (EBO_) glDeleteBuffers(1, &EBO_);
    doneCurrent();
}


void OpenGLWidget::initializeGL() {
    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return;
    }

    initializeOpenGLFunctions();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Load and compile shaders
    if (!shader_.loadFromString(vertexShaderSource_, fragmentShaderSource_)) {
        std::cerr << "Failed to initialize shaders" << std::endl;
        return;
    }

    // Initialize texture
    glGenTextures(1, &textureId_);
    glBindTexture(GL_TEXTURE_2D, textureId_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Setup quad for rendering
    setupQuad();
}

void OpenGLWidget::setupQuad() {
    // Vertex data for a quad with texture coordinates
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

    glGenVertexArrays(1, &VAO_);
    glGenBuffers(1, &VBO_);
    glGenBuffers(1, &EBO_);

    glBindVertexArray(VAO_);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
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

void OpenGLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT);

    if (hasNewFrame_) {
        updateTexture();
        hasNewFrame_ = false;
    }

    shader_.use();
    shader_.setInt("videoTexture", 0);  // Set texture unit 0

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId_);

    glBindVertexArray(VAO_);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void OpenGLWidget::updateTexture() {
    if (currentFrame_.empty()) return;

    glBindTexture(GL_TEXTURE_2D, textureId_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 currentFrame_.cols, currentFrame_.rows, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, currentFrame_.data);
}


void OpenGLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
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