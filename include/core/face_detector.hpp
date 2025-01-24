// include/core/face_detector.hpp
#pragma once

#include <dlib/image_processing.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <opencv2/opencv.hpp>

namespace capvision {
namespace core {

class FaceDetector {
public:
    struct FaceDetectionResult {
        std::vector<cv::Point2f> landmarks;  // 68 facial landmarks
        cv::Mat rotation_matrix;             // 3x3 rotation matrix
        cv::Vec3d euler_angles;              // Pitch, Yaw, Roll
        cv::Rect face_rect;                  // Face bounding box
        bool success{false};
    };

    FaceDetector();
    ~FaceDetector();

    bool initialize();
    FaceDetectionResult detectFace(const cv::Mat& frame);

private:
    // DLib's face detector
    dlib::frontal_face_detector detector_;
    
    // DLib's shape predictor for facial landmarks
    dlib::shape_predictor shape_predictor_;
    
    // Explicit Model path
    const std::string model_path_{"D:/enhanced_projects/cap_vision/resources/models/shape_predictor_68_face_landmarks.dat"};
    
    // 3D model points for pose estimation
    std::vector<cv::Point3d> model_points_3d_;
    
    // Camera matrix (will be initialized based on image size)
    cv::Mat camera_matrix_;
    
    // Distortion coefficients (assumed to be zero for webcam)
    cv::Mat dist_coeffs_ = cv::Mat::zeros(4, 1, cv::DataType<double>::type);
    
    bool initialized_{false};
};

} // namespace core
} // namespace capvision