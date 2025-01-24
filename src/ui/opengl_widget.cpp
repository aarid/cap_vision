#include "../../include/ui/opengl_widget.hpp"

namespace capvision {
namespace ui {

OpenGLWidget::OpenGLWidget(QWidget* parent)
    : QOpenGLWidget(parent)
{
}

OpenGLWidget::~OpenGLWidget()
{
    makeCurrent();
    if (textureId_) {
        glDeleteTextures(1, &textureId_);
    }
    doneCurrent();
}

void OpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Initialize texture
    glGenTextures(1, &textureId_);
    glBindTexture(GL_TEXTURE_2D, textureId_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void OpenGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);

    if (hasNewFrame_) {
        updateTexture();
        hasNewFrame_ = false;
    }

    // Basic texture display
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureId_);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, -1.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex2f( 1.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex2f( 1.0f,  1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f,  1.0f);
    glEnd();

    glDisable(GL_TEXTURE_2D);
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

void OpenGLWidget::updateTexture()
{
    if (currentFrame_.empty()) return;

    glBindTexture(GL_TEXTURE_2D, textureId_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 currentFrame_.cols, currentFrame_.rows, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, currentFrame_.data);
}

} // namespace ui
} // namespace capvision