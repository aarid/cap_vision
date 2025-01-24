#pragma once

#include <QtOpenGLWidgets/QOpenGLWidget>
#include <QtGui/QOpenGLFunctions>
#include <opencv2/opencv.hpp>
#include "../../include/core/face_detector.hpp"

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
};

} // namespace ui
} // namespace capvision