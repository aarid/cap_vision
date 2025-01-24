cmake_minimum_required(VERSION 3.15)
project(CapVision VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Auto MOC for Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Explicit path for libs
set(OpenCV_DIR "D:/Packages/opencv-4.10.0/build")
set(Qt6_DIR "D:/Programs/Qt/6.7.3/msvc2022_64/lib/cmake/Qt6")
set(GLEW_ROOT "D:/Programs/glew-2.1.0")


# Config lib GLEW for OpenGL 
set(GLEW_INCLUDE_DIR "${GLEW_ROOT}/include")
set(GLEW_LIB "${GLEW_ROOT}/lib/Release/x64/glew32.lib")


# File source recurse retrieve
file(GLOB_RECURSE LIB_SOURCES
    "src/*.cpp"
)

file(GLOB_RECURSE HEADERS
    "include/*.hpp"
    "include/*.h"
)

# Find required packages
find_package(OpenCV REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Qt6 COMPONENTS Core Widgets REQUIRED)

# Include directories
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${OpenCV_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIR}
    ${GLEW_INCLUDE_DIR}
)

# Create executable
add_executable(${PROJECT_NAME} ${LIB_SOURCES} ${HEADERS})


# Link libraries
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    ${OpenCV_LIBS}
    ${OPENGL_LIBRARIES}
    ${GLEW_LIB}
    Qt::Core
    Qt::Widgets
)