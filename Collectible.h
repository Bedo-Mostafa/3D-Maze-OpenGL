//#ifndef COLLECTIBLE_H
//#define COLLECTIBLE_H
//
//#include "Vector3.h"
//#include <ctime>
//#include <vector>
//
//class Collectible {
//public:
//    Collectible(const Vector3& position, float radius = 0.3f);
//
//    void update(float deltaTime);
//
//    bool isCollected() const { return m_collected; }
//    void collect() { m_collected = true; }
//
//    Vector3 getPosition() const;
//    float getRadius() const { return m_radius; }
//    float getHoverOffset() const { return m_hoverOffset; }
//    float getRotation() const { return m_rotation; }
//    float getPulse() const { return m_pulse; }
//    // NEW: Get base position without hover offset
//    Vector3 getBasePosition() const { return m_basePosition; }
//
//private:
//    Vector3 m_basePosition;
//    float m_radius;
//    float m_hoverOffset;
//    float m_rotation;
//    float m_pulse;
//    float m_time;
//    bool m_collected;
//};
//
//// Collection manager
//class CollectibleManager {
//public:
//    CollectibleManager();
//
//    void initialize(const class Maze& maze);
//    void update(float deltaTime);
//    void checkCollisions(const Vector3& playerPos, float playerRadius);
//
//    int getCollectedCount() const { return m_collectedCount; }
//    int getTotalCount() const { return static_cast<int>(m_collectibles.size()); }
//    bool allCollected() const { return m_collectedCount >= static_cast<int>(m_collectibles.size()); }
//
//    const std::vector<Collectible>& getCollectibles() const { return m_collectibles; }
//
//    // For particle effect trigger
//    bool hasNewCollection() {
//        bool result = m_newCollection;
//        m_newCollection = false;
//        return result;
//    }
//
//    Vector3 getLastCollectionPos() const { return m_lastCollectionPos; }
//
//private:
//    std::vector<Collectible> m_collectibles;
//    int m_collectedCount;
//    bool m_newCollection;
//    Vector3 m_lastCollectionPos;
//};
//
//#endif // COLLECTIBLE_H


#pragma once
#include "Vector3.h"
#include <vector>

class Maze;

class Collectible {
public:
    Collectible(const Vector3& position, float radius);

    void update(float deltaTime);
    Vector3 getPosition() const;

    // NEW: Get base position without hover offset (for minimap and particles)
    Vector3 getBasePosition() const { return m_basePosition; }

    float getRadius() const { return m_radius; }
    float getRotation() const { return m_rotation; }
    float getPulse() const { return m_pulse; }

    bool isCollected() const { return m_collected; }
    void collect() { m_collected = true; }

private:
    Vector3 m_basePosition;  // Static world position
    float m_radius;
    float m_hoverOffset;     // Animated vertical offset
    float m_rotation;
    float m_pulse;
    float m_time;
    bool m_collected;
};

class CollectibleManager {
public:
    CollectibleManager();

    void initialize(const Maze& maze);
    void update(float deltaTime);
    void checkCollisions(const Vector3& playerPos, float playerRadius);

    int getCollectedCount() const { return m_collectedCount; }
    int getTotalCount() const { return static_cast<int>(m_collectibles.size()); }

    const std::vector<Collectible>& getCollectibles() const { return m_collectibles; }

    bool hasNewCollection() {
        bool result = m_newCollection;
        m_newCollection = false;
        return result;
    }

    Vector3 getLastCollectionPos() const { return m_lastCollectionPos; }

private:
    std::vector<Collectible> m_collectibles;
    int m_collectedCount;
    bool m_newCollection;
    Vector3 m_lastCollectionPos;
};