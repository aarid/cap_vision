// src/ui/face_visualization.cpp
#include "../../include/ui/face_visualizer.hpp"
#include <sstream>
#include <iomanip>

namespace capvision {
namespace ui {

void FaceVisualizer::drawFaceInfo(cv::Mat& frame,
                                    const core::FaceDetector::FaceDetectionResult& result,
                                    const Options& options) {
    if (!result.success) return;

    if (options.showFaceRect) {
        cv::rectangle(frame, result.face_rect, options.rectangleColor, options.lineThickness);
    }

    if (options.showLandmarks) {
        drawLandmarks(frame, result.landmarks, options);
        drawFaceConnections(frame, result.landmarks, options);
    }

    if (options.showPoseAxes) {
        drawPoseAxes(frame, result, options);
    }

    if (options.showEulerAngles) {
        drawEulerAngles(frame, result.euler_angles, options);
    }
}

void FaceVisualizer::drawLandmarks(cv::Mat& frame,
                                     const std::vector<cv::Point2f>& landmarks,
                                     const Options& options) {
    for (const auto& point : landmarks) {
        cv::circle(frame, point, options.landmarkRadius, 
                  options.landmarkColor, -1);
    }
}

void FaceVisualizer::drawFaceConnections(cv::Mat& frame,
                                           const std::vector<cv::Point2f>& landmarks,
                                           const Options& options) {
    // Jaw line
    for (int i = 0; i < 16; i++) {
        cv::line(frame, landmarks[i], landmarks[i+1], 
                options.connectionColor, options.lineThickness);
    }

    // Eyebrows
    for (int i = 17; i < 21; i++) {
        cv::line(frame, landmarks[i], landmarks[i+1], 
                options.connectionColor, options.lineThickness);
    }
    for (int i = 22; i < 26; i++) {
        cv::line(frame, landmarks[i], landmarks[i+1], 
                options.connectionColor, options.lineThickness);
    }

    // Nose
    for (int i = 27; i < 30; i++) {
        cv::line(frame, landmarks[i], landmarks[i+1], 
                options.connectionColor, options.lineThickness);
    }
    for (int i = 31; i < 35; i++) {
        cv::line(frame, landmarks[i], landmarks[i+1], 
                options.connectionColor, options.lineThickness);
    }
    cv::line(frame, landmarks[35], landmarks[30], 
            options.connectionColor, options.lineThickness);

    // Eyes
    for (int i = 36; i < 41; i++) {
        cv::line(frame, landmarks[i], landmarks[i+1], 
                options.connectionColor, options.lineThickness);
    }
    cv::line(frame, landmarks[41], landmarks[36], 
            options.connectionColor, options.lineThickness);

    for (int i = 42; i < 47; i++) {
        cv::line(frame, landmarks[i], landmarks[i+1], 
                options.connectionColor, options.lineThickness);
    }
    cv::line(frame, landmarks[47], landmarks[42], 
            options.connectionColor, options.lineThickness);

    // Mouth
    for (int i = 48; i < 59; i++) {
        cv::line(frame, landmarks[i], landmarks[i+1], 
                options.connectionColor, options.lineThickness);
    }
    cv::line(frame, landmarks[59], landmarks[48], 
            options.connectionColor, options.lineThickness);

    for (int i = 60; i < 67; i++) {
        cv::line(frame, landmarks[i], landmarks[i+1], 
                options.connectionColor, options.lineThickness);
    }
    cv::line(frame, landmarks[67], landmarks[60], 
            options.connectionColor, options.lineThickness);
}

void FaceVisualizer::drawPoseAxes(cv::Mat& frame,
                                    const core::FaceDetector::FaceDetectionResult& result,
                                    const Options& options) {
    if (result.landmarks.empty()) return;

    cv::Point2f nose = result.landmarks[30];
    float axisLength = 50.0f;
    
    std::vector<cv::Point2f> projectedPoints;
    std::vector<cv::Point3f> axisPoints = {
        cv::Point3f(0.0f, 0.0f, 0.0f),
        cv::Point3f(axisLength, 0.0f, 0.0f),
        cv::Point3f(0.0f, -axisLength, 0.0f),
        cv::Point3f(0.0f, 0.0f, -axisLength)
    };

    cv::projectPoints(axisPoints, 
                     cv::Mat(result.euler_angles), 
                     cv::Mat::zeros(3, 1, CV_64F),
                     result.rotation_matrix,
                     cv::Mat::zeros(4, 1, CV_64F),
                     projectedPoints);

    // X, Y, Z axes with fixed colors
    cv::line(frame, nose, projectedPoints[1], cv::Scalar(0, 0, 255), 2);   // X: Red
    cv::line(frame, nose, projectedPoints[2], cv::Scalar(0, 255, 0), 2);   // Y: Green
    cv::line(frame, nose, projectedPoints[3], cv::Scalar(255, 0, 0), 2);   // Z: Blue
}

void FaceVisualizer::drawEulerAngles(cv::Mat& frame,
                                       const cv::Vec3d& euler_angles,
                                       const Options& options) {
    double yaw = euler_angles[0] * 180.0 / CV_PI;
    double pitch = euler_angles[1] * 180.0 / CV_PI;
    double roll = euler_angles[2] * 180.0 / CV_PI;

    std::stringstream ss;
    ss << std::fixed << std::setprecision(1)
       << "Yaw: " << yaw << "°, Pitch: " << pitch << "°, Roll: " << roll << "°";
    
    cv::putText(frame, ss.str(), cv::Point(10, 30),
               cv::FONT_HERSHEY_SIMPLEX, 0.7, options.connectionColor, 2);
}

} // namespace ui
} // namespace capvision