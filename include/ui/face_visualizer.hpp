#pragma once

#include <opencv2/opencv.hpp>
#include "../../include/core/face_detector.hpp"

namespace capvision {
namespace ui {

class FaceVisualizer {
public:
    // Visualization options structure
    struct Options {
        bool showLandmarks{true};
        bool showPoseAxes{true};
        bool showFaceRect{true};
        bool showEulerAngles{true};
        
        // Visualization parameters
        cv::Scalar landmarkColor{0, 0, 255};    // Red
        cv::Scalar connectionColor{0, 255, 0};  // Green
        cv::Scalar rectangleColor{0, 255, 0};   // Green
        int landmarkRadius{2};
        int lineThickness{1};
    };

    // Static drawing functions using Options
    static void drawFaceInfo(cv::Mat& frame, 
                           const core::FaceDetector::FaceDetectionResult& result,
                           const Options& options);

    static void drawLandmarks(cv::Mat& frame, 
                            const std::vector<cv::Point2f>& landmarks,
                            const Options& options);

    static void drawFaceConnections(cv::Mat& frame, 
                                  const std::vector<cv::Point2f>& landmarks,
                                  const Options& options);

    static void drawPoseAxes(cv::Mat& frame, 
                            const core::FaceDetector::FaceDetectionResult& result,
                            const Options& options);

    static void drawEulerAngles(cv::Mat& frame, 
                               const cv::Vec3d& euler_angles,
                               const Options& options);
};

} // namespace ui
} // namespace capvision