#ifndef CAMERA_H
#define CAMERA_H

#include "Vector3.h"
#include <array>

/**
 * @class Camera
 * @brief Manages FPS camera with position, orientation, and view/projection matrices
 * 
 * Single Responsibility: Handles camera transformations and movement
 */
class Camera {
public:
    Camera(const Vector3& position = Vector3(0.0f, 0.0f, 0.0f),
           float yaw = -90.0f, float pitch = 0.0f);

    // Camera movement
    void moveForward(float distance);
    void moveBackward(float distance);
    void moveLeft(float distance);
    void moveRight(float distance);

    // Camera rotation
    void rotate(float deltaYaw, float deltaPitch);

    // Getters
    Vector3 getPosition() const { return m_position; }
    Vector3 getFront() const { return m_front; }
    Vector3 getUp() const { return m_up; }
    Vector3 getRight() const { return m_right; }

    /**
     * @brief Get view matrix (LookAt matrix)
     */
    std::array<float, 16> getViewMatrix() const;

    /**
     * @brief Get projection matrix (perspective)
     */
    std::array<float, 16> getProjectionMatrix(float fov, float aspect, float near, float far) const;

    // Setters
    void setPosition(const Vector3& position) { m_position = position; }

private:
    Vector3 m_position;
    Vector3 m_front;
    Vector3 m_up;
    Vector3 m_right;
    Vector3 m_worldUp;

    float m_yaw;
    float m_pitch;

    /**
     * @brief Update camera vectors based on yaw and pitch
     */
    void updateCameraVectors();
};

#endif // CAMERA_H