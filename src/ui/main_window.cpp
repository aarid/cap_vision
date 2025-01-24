#include "../../include/ui/main_window.hpp"
#include <QtCore/QTimer>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QtWidgets>
#include <QtWidgets/QLabel>

namespace capvision {
namespace ui {

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi();
    initializeCamera();
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUi() {
    auto centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    auto layout = new QVBoxLayout(centralWidget);
    
    videoWidget_ = new VideoWidget(this);
    layout->addWidget(videoWidget_);
    
    resize(800, 600);
    
    updateTimer_ = new QTimer(this);
    connect(updateTimer_, &QTimer::timeout, this, &MainWindow::updateFrame);
    updateTimer_->start(33);
}

void MainWindow::initializeCamera() {
    camera_.open(0);
    if (!camera_.isOpened()) {
        // TODO: Gérer l'erreur
        return;
    }
    
    faceDetector_.initialize();
}

void MainWindow::updateFrame() {
    cv::Mat frame;
    if (!camera_.read(frame)) {
        return;
    }
    
    cv::Rect faceRect;
    if (faceDetector_.detectFace(frame, faceRect)) {
        cv::rectangle(frame, faceRect, cv::Scalar(0, 255, 0), 2);
    }
    
    videoWidget_->updateFrame(frame);
}

} // namespace ui
} // namespace capvision