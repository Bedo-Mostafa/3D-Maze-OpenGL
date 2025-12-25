#ifndef PLAYER_H
#define PLAYER_H

#include "Camera.h"
#include "Vector3.h"
#include "Maze.h"
#include <iostream>

/**
 * @class Player
 * @brief First-person player controller with collision and movement
 * 
 * Handles player physics, movement, and camera as a unified FPS character
 */
class Player {
public:
    Player(const Vector3& startPosition);

    /**
     * @brief Update player movement and physics
     * @param deltaTime Time since last frame
     * @param forward Move forward input (-1.0 to 1.0)
     * @param right Move right input (-1.0 to 1.0)
     * @param maze Reference to maze for collision detection
     */
    void update(float deltaTime, float forward, float right, const Maze& maze);

    /**
     * @brief Rotate player view (mouse look)
     * @param deltaYaw Horizontal rotation change
     * @param deltaPitch Vertical rotation change
     */
    void rotate(float deltaYaw, float deltaPitch);

    /**
     * @brief Get the player's camera for rendering
     */
    Camera& getCamera() { return m_camera; }
    const Camera& getCamera() const { return m_camera; }

    /**
     * @brief Get player's position (feet level)
     */
    Vector3 getPosition() const { return m_position; }

    /**
     * @brief Get player's eye position (camera position)
     */
    Vector3 getEyePosition() const;

    /**
     * @brief Set player position (teleport)
     */
    void setPosition(const Vector3& position);
    float getCollisionRadius() const { return m_collisionRadius; }

private:
    // Player physical properties
    Vector3 m_position;           // Position at feet level
    float m_eyeHeight;            // Height from feet to eyes
    float m_collisionRadius;      // Collision cylinder radius
    float m_height;               // Total player height

    // Movement properties
    float m_moveSpeed;            // Movement speed in units/sec
    float m_sprintMultiplier;     // Sprint speed multiplier
    Vector3 m_velocity;           // Current velocity

    // Camera for rendering
    Camera m_camera;

    /**
     * @brief Apply movement with collision detection
     */
    Vector3 calculateMovement(float forward, float right, float deltaTime) const;

    /**
     * @brief Check and resolve collisions with maze
     */
    Vector3 resolveCollision(const Vector3& desiredPosition, const Maze& maze) const;

    /**
     * @brief Update camera position to match player eye height
     */
    void updateCameraPosition();

    float m_walkDistance = 0.0f; // Add this
};

#endif // PLAYER_H
