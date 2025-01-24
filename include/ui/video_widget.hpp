#pragma once

#include <QtWidgets/QWidget>
#include <QtWidgets/QLabel>
#include <opencv2/opencv.hpp>

namespace capvision {
namespace ui {

class VideoWidget : public QWidget {
    Q_OBJECT

public:
    explicit VideoWidget(QWidget* parent = nullptr);
    ~VideoWidget();

    void updateFrame(const cv::Mat& frame);
    QSize getPreferredSize() const;

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    QLabel* displayLabel_;
    QSize preferredSize_;
};

} // namespace ui
} // namespace capvision