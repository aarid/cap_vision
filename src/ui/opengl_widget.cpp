#include "../../include/ui/opengl_widget.hpp"

namespace capvision {
namespace ui {

OpenGLWidget::OpenGLWidget(QWidget* parent)
    : QOpenGLWidget(parent)
{
}

OpenGLWidget::~OpenGLWidget() {
    makeCurrent();
    if (textureId_) glDeleteTextures(1, &textureId_);
    if (quadVAO_) glDeleteVertexArrays(1, &quadVAO_);
    if (quadVBO_) glDeleteBuffers(1, &quadVBO_);
    if (quadEBO_) glDeleteBuffers(1, &quadEBO_);
    doneCurrent();
}

void OpenGLWidget::initializeGL() {
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return;
    }

    initializeOpenGLFunctions();
    
    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Setup shaders
    setupShaders();

    // Setup quad for video rendering
    setupQuad();

    // Initialize texture
    glGenTextures(1, &textureId_);
    glBindTexture(GL_TEXTURE_2D, textureId_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


    // Load 3D model
    try {
        model_ = std::make_unique<Model3D>("resources/models/cap/10131_BaseballCap_v2_L3.obj");  // Ajustez le chemin selon votre modèle
    } catch (const std::exception& e) {
        std::cerr << "Failed to load model: " << e.what() << std::endl;
    }

    // Initialize view matrix
    view_ = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 3.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
}


void OpenGLWidget::setupShaders() {
    if (!videoShader_.loadFromString(videoVertexShaderSource_, videoFragmentShaderSource_)) {
        std::cerr << "Failed to load video shaders" << std::endl;
        return;
    }

    if (!modelShader_.loadFromString(modelVertexShaderSource_, modelFragmentShaderSource_)) {
        std::cerr << "Failed to load model shaders" << std::endl;
        return;
    }
}

void OpenGLWidget::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
    aspectRatio_ = static_cast<float>(w) / static_cast<float>(h);
    
    // Update projection matrix
    projection_ = glm::perspective(glm::radians(45.0f), aspectRatio_, 0.1f, 100.0f);
}

void OpenGLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render video background
    renderVideo();

    // Render 3D model (will be implemented)
    renderModel();
}

void OpenGLWidget::renderVideo() {
    if (hasNewFrame_) {
        updateTexture();
        hasNewFrame_ = false;
    }

    glDisable(GL_DEPTH_TEST);  // Disable depth testing for video
    
    videoShader_.use();
    videoShader_.setInt("videoTexture", 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId_);

    glBindVertexArray(quadVAO_);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);  // Re-enable depth testing for 3D
}

void OpenGLWidget::renderModel() {
    if (!model_ || !faceResult_.success) return;

    modelShader_.use();

    // Update model matrix based on face detection results
    modelMatrix_ = glm::mat4(1.0f);
    
    // Position the model at the nose position
    glm::vec3 nosePosition(
        faceResult_.landmarks[30].x,
        faceResult_.landmarks[30].y,
        0.0f
    );
    
    // Convert from screen space to world space
    float screenX = (nosePosition.x / width() - 0.5f) * 2.0f;
    float screenY = -(nosePosition.y / height() - 0.5f) * 2.0f;
    
    modelMatrix_ = glm::translate(modelMatrix_, glm::vec3(screenX, screenY, 0.0f));
    
    // Apply rotation from face detection
    glm::mat4 rotationMatrix(faceResult_.rotation_matrix.at<double>(0,0), faceResult_.rotation_matrix.at<double>(0,1), faceResult_.rotation_matrix.at<double>(0,2), 0.0f,
                            faceResult_.rotation_matrix.at<double>(1,0), faceResult_.rotation_matrix.at<double>(1,1), faceResult_.rotation_matrix.at<double>(1,2), 0.0f,
                            faceResult_.rotation_matrix.at<double>(2,0), faceResult_.rotation_matrix.at<double>(2,1), faceResult_.rotation_matrix.at<double>(2,2), 0.0f,
                            0.0f, 0.0f, 0.0f, 1.0f);
    
    modelMatrix_ *= rotationMatrix;
    
    // Apply scale
    modelMatrix_ = glm::scale(modelMatrix_, glm::vec3(modelScale_));

    // Set shader uniforms
    modelShader_.setMat4("projection", glm::value_ptr(projection_));
    modelShader_.setMat4("view", glm::value_ptr(view_));
    modelShader_.setMat4("model", glm::value_ptr(modelMatrix_));

    // Set lighting uniforms
    modelShader_.setVec3("lightPos", 0.0f, 0.0f, 2.0f);
    modelShader_.setVec3("viewPos", 0.0f, 0.0f, 3.0f);

    // Render the model
    model_->render(modelShader_);
}

void OpenGLWidget::setupQuad() {
    float vertices[] = {
        // positions        // texture coords
        -1.0f,  1.0f, 0.0f,  0.0f, 0.0f,  // top left
         1.0f,  1.0f, 0.0f,  1.0f, 0.0f,  // top right
         1.0f, -1.0f, 0.0f,  1.0f, 1.0f,  // bottom right
        -1.0f, -1.0f, 0.0f,  0.0f, 1.0f   // bottom left
    };

    unsigned int indices[] = {
        0, 1, 2,  // first triangle
        0, 2, 3   // second triangle
    };

    glGenVertexArrays(1, &quadVAO_);
    glGenBuffers(1, &quadVBO_);
    glGenBuffers(1, &quadEBO_);

    glBindVertexArray(quadVAO_);

    glBindBuffer(GL_ARRAY_BUFFER, quadVBO_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Texture coordinate attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void OpenGLWidget::updateTexture() {
    if (currentFrame_.empty()) return;

    glBindTexture(GL_TEXTURE_2D, textureId_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 currentFrame_.cols, currentFrame_.rows, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, currentFrame_.data);
}


void OpenGLWidget::updateFrame(const cv::Mat& frame,
                             const core::FaceDetector::FaceDetectionResult& face)
{
    if (frame.empty()) return;

    cv::Mat temp;
    cv::cvtColor(frame, temp, cv::COLOR_BGR2RGB);
    currentFrame_ = temp.clone();
    faceResult_ = face;
    hasNewFrame_ = true;
    update(); // Trigger repaint
}

} // namespace ui
} // namespace capvision