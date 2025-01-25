# CapVision

A basic AR cap try-on prototype using OpenCV, DLib, and OpenGL. This is an initial version that will be reimplemented using MediaPipe for better 3D face mesh detection.

## Dependencies

All dependencies can be installed using vcpkg:

```bash
vcpkg install opencv:x64-windows
vcpkg install dlib:x64-windows
vcpkg install glew:x64-windows
vcpkg install glm:x64-windows
vcpkg install qt6:x64-windows
vcpkg install stb:x64-windows
```

## Project Status

This is a prototype version demonstrating:
- Face detection and landmarks using DLib
- Basic OpenGL rendering
- Qt-based UI

The project will be reimplemented using MediaPipe for improved 3D face mesh detection.