#include "Camera.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Camera::Camera(const Vector3& position, float yaw, float pitch)
    : m_position(position),
    m_worldUp(0.0f, 1.0f, 0.0f),
    m_yaw(yaw),
    m_pitch(pitch) {
    updateCameraVectors();
}

void Camera::moveForward(float distance) {
    // Move only on horizontal plane (ignore Y component)
    Vector3 horizontalFront = m_front;
    horizontalFront.y = 0.0f;
    float length = horizontalFront.length();
    if (length > 0.0f) {
        horizontalFront = horizontalFront * (1.0f / length);
    }
    m_position += horizontalFront * distance;
}

void Camera::moveBackward(float distance) {
    // Move only on horizontal plane (ignore Y component)
    Vector3 horizontalFront = m_front;
    horizontalFront.y = 0.0f;
    float length = horizontalFront.length();
    if (length > 0.0f) {
        horizontalFront = horizontalFront * (1.0f / length);
    }
    m_position -= horizontalFront * distance;
}

void Camera::moveLeft(float distance) {
    m_position -= m_right * distance;
}

void Camera::moveRight(float distance) {
    m_position += m_right * distance;
}

void Camera::rotate(float deltaYaw, float deltaPitch) {
    m_yaw += deltaYaw;
    m_pitch += deltaPitch;

    // Constrain pitch to avoid gimbal lock
    if (m_pitch > 89.0f) m_pitch = 89.0f;
    if (m_pitch < -89.0f) m_pitch = -89.0f;

    updateCameraVectors();
}

void Camera::updateCameraVectors() {
    // Calculate new front vector
    float yawRad = m_yaw * M_PI / 180.0f;
    float pitchRad = m_pitch * M_PI / 180.0f;

    Vector3 front;
    front.x = std::cos(yawRad) * std::cos(pitchRad);
    front.y = std::sin(pitchRad);
    front.z = std::sin(yawRad) * std::cos(pitchRad);
    m_front = front.normalize();

    // Calculate right and up vectors
    m_right = m_front.cross(m_worldUp).normalize();
    m_up = m_right.cross(m_front).normalize();
}

std::array<float, 16> Camera::getViewMatrix() const {
    Vector3 f = m_front.normalize();
    Vector3 r = m_right.normalize();
    Vector3 u = m_up.normalize();

    std::array<float, 16> view = {
        r.x,  u.x, -f.x,  0.0f,  // Column 0
        r.y,  u.y, -f.y,  0.0f,  // Column 1
        r.z,  u.z, -f.z,  0.0f,  // Column 2
        -r.dot(m_position), -u.dot(m_position), f.dot(m_position), 1.0f // Column 3
    };

    return view;
}

std::array<float, 16> Camera::getProjectionMatrix(float fov, float aspect, float near, float far) const {
    // Perspective projection matrix (column-major for OpenGL)
    float tanHalfFov = std::tan(fov * M_PI / 360.0f);

    // CRITICAL: Proper column-major projection matrix
    std::array<float, 16> proj = {
        1.0f / (aspect * tanHalfFov), 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f / tanHalfFov, 0.0f, 0.0f,
        0.0f, 0.0f, -(far + near) / (far - near), -1.0f,
        0.0f, 0.0f, -(2.0f * far * near) / (far - near), 0.0f
    };

    return proj;
}