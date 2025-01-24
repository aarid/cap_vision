#pragma once

#include <QtWidgets/QMainWindow>
#include <opencv2/opencv.hpp>
#include "../../include/ui/opengl_widget.hpp"
#include "../../include/core/face_detector.hpp"
#include "../../include/ui/face_visualizer.hpp"

namespace capvision {
namespace ui {

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void updateFrame();

private:
    void setupUi();
    void initializeCamera();

    // UI components
    OpenGLWidget* openglWidget_{nullptr};
    QTimer* updateTimer_{nullptr};

    // Core components
    cv::VideoCapture camera_;
    core::FaceDetector faceDetector_;

    // Visualization options
    FaceVisualizer::Options visualizerOptions_;
};

} // namespace ui
} // namespace capvision