#include "../../include/ui/video_widget.hpp"
#include <QtWidgets/QVBoxLayout>
#include <QtGui/QResizeEvent>
#include <QtWidgets/QLabel>

namespace capvision {
namespace ui {

VideoWidget::VideoWidget(QWidget* parent)
    : QWidget(parent)
    , displayLabel_(new QLabel(this))
    , preferredSize_(640, 480)
{
    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(displayLabel_);

    displayLabel_->setMinimumSize(preferredSize_);
    displayLabel_->setAlignment(Qt::AlignCenter);
}

VideoWidget::~VideoWidget() = default;

void VideoWidget::updateFrame(const cv::Mat& frame) {
    if (frame.empty()) return;

    cv::Mat rgbFrame;
    cv::cvtColor(frame, rgbFrame, cv::COLOR_BGR2RGB);
    
    QImage qimg(rgbFrame.data, rgbFrame.cols, rgbFrame.rows, 
                rgbFrame.step, QImage::Format_RGB888);
    
    displayLabel_->setPixmap(QPixmap::fromImage(qimg).scaled(
        displayLabel_->size(), Qt::KeepAspectRatio));
}

QSize VideoWidget::getPreferredSize() const {
    return preferredSize_;
}

void VideoWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    displayLabel_->setMinimumSize(event->size());
}

} // namespace ui
} // namespace capvision