#include "../../include/ui/main_window.hpp"
#include "../../include/core/face_detector.hpp"
#include <QtCore/QTimer>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QtWidgets>
#include <QtWidgets/QLabel>

namespace capvision {
namespace ui {

class MainWindow::MainWindowPrivate {
public:
    QLabel* videoLabel{nullptr};
    cv::VideoCapture camera;
    core::FaceDetector faceDetector;
    QTimer* updateTimer{nullptr};
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , d_ptr(new MainWindowPrivate)
{
    setupUi();
    initializeCamera();
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUi() {
    auto centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    auto layout = new QVBoxLayout(centralWidget);
    
    d_ptr->videoLabel = new QLabel(this);
    layout->addWidget(d_ptr->videoLabel);
    
    resize(800, 600);
    
    d_ptr->updateTimer = new QTimer(this);
    connect(d_ptr->updateTimer, &QTimer::timeout, this, &MainWindow::updateFrame);
    d_ptr->updateTimer->start(33);
}

void MainWindow::initializeCamera() {
    d_ptr->camera.open(0);
    if (!d_ptr->camera.isOpened()) {
        return;
    }
    
    d_ptr->faceDetector.initialize();
}

void MainWindow::updateFrame() {
    cv::Mat frame;
    if (!d_ptr->camera.read(frame)) {
        return;
    }
    
    cv::Rect faceRect;
    if (d_ptr->faceDetector.detectFace(frame, faceRect)) {
        cv::rectangle(frame, faceRect, cv::Scalar(0, 255, 0), 2);
    }
    
    cv::Mat rgbFrame;
    cv::cvtColor(frame, rgbFrame, cv::COLOR_BGR2RGB);
    QImage qimg(rgbFrame.data, rgbFrame.cols, rgbFrame.rows, 
                rgbFrame.step, QImage::Format_RGB888);
    
    d_ptr->videoLabel->setPixmap(QPixmap::fromImage(qimg).scaled(
        d_ptr->videoLabel->size(), Qt::KeepAspectRatio));
}

} // namespace ui
} // namespace capvision