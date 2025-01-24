#include "../../include/core/face_detector.hpp"
#include <dlib/opencv.h>

namespace capvision {
namespace core {

FaceDetector::FaceDetector() : detector_(dlib::get_frontal_face_detector()) {
    // Initialize 3D model points for pose estimation
    model_points_3d_ = {
        cv::Point3d(0.0, 0.0, 0.0),          // Nose tip
        cv::Point3d(0.0, -330.0, -65.0),     // Chin
        cv::Point3d(-225.0, 170.0, -135.0),  // Left eye corner
        cv::Point3d(225.0, 170.0, -135.0),   // Right eye corner
        cv::Point3d(-150.0, -150.0, -125.0), // Left mouth corner
        cv::Point3d(150.0, -150.0, -125.0)   // Right mouth corner
    };
}

FaceDetector::~FaceDetector() = default;

bool FaceDetector::initialize() {
    try {
        // Load face landmark detector
        dlib::deserialize(model_path_) >> shape_predictor_;
        initialized_ = true;
        return true;
    }
    catch (const dlib::serialization_error& e) {
        std::cerr << "Failed to load shape predictor model: " << e.what() << std::endl;
        return false;
    }
}

FaceDetector::FaceDetectionResult FaceDetector::detectFace(const cv::Mat& frame) {
    FaceDetectionResult result;
    result.success = false;
    
    if (!initialized_ || frame.empty()) {
        return result;
    }

    // Convert OpenCV Mat to dlib matrix
    dlib::matrix<dlib::rgb_pixel> dlib_img;
    dlib::assign_image(dlib_img, dlib::cv_image<dlib::bgr_pixel>(frame));

    // Detect faces
    std::vector<dlib::rectangle> faces = detector_(dlib_img);
    if (faces.empty()) {
        return result;
    }

    // Get the first face
    auto face = faces[0];
    result.face_rect = cv::Rect(face.left(), face.top(), face.width(), face.height());

    // Detect landmarks
    auto shape = shape_predictor_(dlib_img, face);
    result.landmarks.reserve(68);

    // Convert landmarks to OpenCV format
    for (unsigned int i = 0; i < shape.num_parts(); ++i) {
        auto point = shape.part(i);
        result.landmarks.emplace_back(point.x(), point.y());
    }

    // Initialize camera matrix if needed
    if (camera_matrix_.empty()) {
        float focal_length = frame.cols;
        cv::Point2d center(frame.cols/2, frame.rows/2);
        camera_matrix_ = (cv::Mat_<double>(3, 3) << 
            focal_length, 0, center.x,
            0, focal_length, center.y,
            0, 0, 1);
    }

    // Get specific facial landmarks for pose estimation
    std::vector<cv::Point2d> image_points;
    image_points.push_back(result.landmarks[30]);    // Nose tip
    image_points.push_back(result.landmarks[8]);     // Chin
    image_points.push_back(result.landmarks[36]);    // Left eye corner
    image_points.push_back(result.landmarks[45]);    // Right eye corner
    image_points.push_back(result.landmarks[48]);    // Left mouth corner
    image_points.push_back(result.landmarks[54]);    // Right mouth corner


    // Solve for pose
    cv::Mat rvec, tvec;
    cv::solvePnP(model_points_3d_, image_points, camera_matrix_, dist_coeffs_, 
                 rvec, tvec, false, cv::SOLVEPNP_ITERATIVE);

    // Convert rotation vector to rotation matrix
    cv::Rodrigues(rvec, result.rotation_matrix);


    // Store rotation vector directly (in degrees)
    // This gives us rotation around X, Y, Z axes directly
    result.euler_angles = cv::Vec3d(
        rvec.at<double>(0) * 180.0 / CV_PI,
        rvec.at<double>(1) * 180.0 / CV_PI,
        rvec.at<double>(2) * 180.0 / CV_PI
    );

    result.success = true;
    return result;
}

} // namespace core
} // namespace capvision