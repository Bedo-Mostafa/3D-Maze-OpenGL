#include "Player.h"
#include <cmath>
#include <algorithm>
#include <iostream>

Player::Player(const Vector3& startPosition)
    : m_position(startPosition),
    m_eyeHeight(1.7f),           // Eye height at 1.7m (standard FPS)
    m_collisionRadius(0.4f),     // Larger radius for better collision
    m_height(1.8f),              // Total height (1.8m = ~5'11")
    m_moveSpeed(5.0f),           // Walking speed (matches original)
    m_sprintMultiplier(1.8f),    // Sprint multiplier
    m_velocity(0.0f, 0.0f, 0.0f),
    m_camera(getEyePosition())   // Camera at eye level
{
    // Force player to ground level
    m_position.y = 0.0f;
    updateCameraPosition();
}

void Player::update(float deltaTime, float forward, float right, const Maze& maze) {
    // Clamp delta time to prevent huge jumps
    if (deltaTime > 0.05f) deltaTime = 0.05f;

    // Get camera direction vectors (flattened to ground plane)
    Vector3 cameraForward = m_camera.getFront();
    cameraForward.y = 0.0f;
    float forwardLen = cameraForward.length();
    if (forwardLen > 0.001f) {
        cameraForward = cameraForward * (1.0f / forwardLen);
    }

    Vector3 cameraRight = m_camera.getRight();
    cameraRight.y = 0.0f;
    float rightLen = cameraRight.length();
    if (rightLen > 0.001f) {
        cameraRight = cameraRight * (1.0f / rightLen);
    }

    // Calculate movement direction
    Vector3 moveDirection(0.0f, 0.0f, 0.0f);
    moveDirection = cameraForward * forward + cameraRight * right;

    // Normalize diagonal movement
    float moveLen = moveDirection.length();
    if (moveLen > 1.0f) {
        moveDirection = moveDirection * (1.0f / moveLen);
    }

    // Apply speed
    float moveSpeed = m_moveSpeed * deltaTime;
    moveDirection = moveDirection * moveSpeed;

    Vector3 pos = m_position;
    int gridX = static_cast<int>(pos.x / 2.0f); // Assuming cell size 2.0
    int gridZ = static_cast<int>(pos.z / 2.0f);

    if (maze.checkCollision(pos, 0.1f)) {
        // 0.1f small radius check to see if center point is in wall
        std::cout << "WARNING: Player center is inside a wall at (" << gridX << "," << gridZ << ")!" << std::endl;
    }

    // Update walk distance if we are moving
    if (forward != 0.0f || right != 0.0f) {
        m_walkDistance += moveSpeed * 2.5f; // 2.5 is the bob frequency
    }
    else {
        m_walkDistance = 0.0f; // Optional: Reset or decay when stopped
    }

    // Current position
    Vector3 currentPosition = m_position;

    // Try X movement first
    Vector3 newPosition = currentPosition;
    newPosition.x += moveDirection.x;

    // Check X collision
    if (maze.checkCollision(newPosition, m_collisionRadius)) {
        newPosition.x = currentPosition.x; // Hit wall on X, revert X
    }

    // Try Z movement (using the result of X movement - allows wall sliding)
    Vector3 zTestPosition = newPosition;
    zTestPosition.z += moveDirection.z;

    // Check Z collision
    if (!maze.checkCollision(zTestPosition, m_collisionRadius)) {
        newPosition.z = zTestPosition.z;
    }

    // Update player position
    m_position = newPosition;

    // CRITICAL: Keep player on ground
    m_position.y = 0.0f;

    // Update camera to match
    updateCameraPosition();
}

void Player::rotate(float deltaYaw, float deltaPitch) {
    m_camera.rotate(deltaYaw, deltaPitch);
}

Vector3 Player::getEyePosition() const {
    // Calculate head bob offset using Sine wave
    // Amplitude 0.1m looks good for walking
    float bobOffset = std::sin(m_walkDistance) * 0.1f;

    return Vector3(m_position.x, m_eyeHeight + bobOffset, m_position.z);
}

void Player::setPosition(const Vector3& position) {
    m_position = position;
    m_position.y = 0.0f; // Always on ground
    updateCameraPosition();
}

void Player::updateCameraPosition() {
    // Set camera at eye height above player position
    Vector3 eyePos = getEyePosition();
    m_camera.setPosition(eyePos);
}