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
    // Create central widget and layout
    auto centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    auto layout = new QVBoxLayout(centralWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    
    // Setup video widget
     openglWidget_ = new OpenGLWidget(this);
    layout->addWidget(openglWidget_);
    
    // Set default window size
    resize(800, 600);
    
    // Setup timer for frame updates
    updateTimer_ = new QTimer(this);
    connect(updateTimer_, &QTimer::timeout, this, &MainWindow::updateFrame);
    updateTimer_->start(33); // ~30 FPS
}

void MainWindow::initializeCamera() {
    // Initialize camera
    camera_.open(0);
    if (!camera_.isOpened()) {
        // TODO: Add error handling
        return;
    }
    
    // Initialize face detector
    if (!faceDetector_.initialize()) {
        // TODO: Add error handling
        exit(1);
    }
}

void MainWindow::updateFrame() {
    cv::Mat frame;
    if (!camera_.read(frame)) {
        return;
    }
    
    // Detect face and get results
    auto result = faceDetector_.detectFace(frame);
    if (result.success) {
        // Draw face information using FaceVisualizer
        FaceVisualizer::drawFaceInfo(frame, result, visualizerOptions_);
    }
    
    // Update display
    openglWidget_->updateFrame(frame, result);
}

} // namespace ui
} // namespace capvision