#pragma once

#include <QtWidgets/QMainWindow>
#include <opencv2/opencv.hpp>
#include "../../include/ui/video_widget.hpp"
#include "../../include/core/face_detector.hpp"

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


    VideoWidget* videoWidget_{nullptr};
    cv::VideoCapture camera_;
    core::FaceDetector faceDetector_;
    QTimer* updateTimer_{nullptr};
};

} // namespace ui
} // namespace capvision