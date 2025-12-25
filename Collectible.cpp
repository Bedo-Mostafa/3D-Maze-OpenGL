#include "Collectible.h"
#include "Maze.h"
#include <cmath>
#include <cstdlib>
#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Collectible::Collectible(const Vector3& position, float radius)
	: m_basePosition(position),
	m_radius(radius),
	m_hoverOffset(0.0f),
	m_rotation(0.0f),
	m_pulse(1.0f),
	m_time(0.0f),
	m_collected(false) {
}

void Collectible::update(float deltaTime) {
	if (m_collected) return;

	m_time += deltaTime;

	// Hovering animation (bob up and down)
	m_hoverOffset = std::sin(m_time * 2.0f) * 0.15f;

	// Rotation animation
	m_rotation = m_time * 90.0f; // 90 degrees per second

	// Pulse animation
	m_pulse = 0.8f + 0.2f * std::sin(m_time * 3.0f);
}

Vector3 Collectible::getPosition() const {
	return Vector3(
		m_basePosition.x,
		m_basePosition.y + m_hoverOffset + 1.0f, // Float at chest/eye level
		m_basePosition.z
	);
}

CollectibleManager::CollectibleManager()
	: m_collectedCount(0),
	m_newCollection(false),
	m_lastCollectionPos(0, 0, 0) {
}

void CollectibleManager::initialize(const Maze& maze) {
	m_collectibles.clear();
	m_collectedCount = 0;
	m_newCollection = false;

	int mazeWidth = maze.getWidth();
	int mazeHeight = maze.getHeight();
	const float cellSize = 2.0f;

	// Place collectibles in empty cells (not start, not end)
	Vector3 startPos = maze.getStartPosition();
	Vector3 endPos = maze.getExitPosition();

	std::srand(static_cast<unsigned int>(std::time(nullptr)));

	int placedCount = 0;
	int maxCollectibles = 10; // Place 10 collectibles

	for (int attempt = 0; attempt < 200 && placedCount < maxCollectibles; ++attempt) {
		// Random cell
		int x = 1 + (std::rand() % (mazeWidth - 2));
		int z = 1 + (std::rand() % (mazeHeight - 2));

		// Check if it's an empty cell
		if (maze.getCellType(x, z) != 0) continue;

		// Calculate world position (center of cell)
		float worldX = (x + 0.5f) * cellSize;
		float worldZ = (z + 0.5f) * cellSize;
		Vector3 pos(worldX, 0.0f, worldZ);

		// Don't place near start or end
		float distToStart = (pos - startPos).length();
		float distToEnd = (pos - endPos).length();

		if (distToStart < 3.0f || distToEnd < 3.0f) continue;

		// Don't place too close to existing collectibles
		bool tooClose = false;
		for (const auto& existing : m_collectibles) {
			float dist = (pos - existing.getPosition()).length();
			if (dist < 3.0f) { // Reduced from 4.0f for more placement options
				tooClose = true;
				break;
			}
		}

		if (tooClose) continue;

		// Place collectible
		m_collectibles.emplace_back(pos, 0.4f); // Increased radius to 0.4f
		placedCount++;
	}
}

void CollectibleManager::update(float deltaTime) {
	for (auto& collectible : m_collectibles) {
		collectible.update(deltaTime);
	}
}

void CollectibleManager::checkCollisions(const Vector3& playerPos, float playerRadius) {

	// Store BASE position (without hover offset) for particle spawn
	for (auto& collectible : m_collectibles) {
		m_lastCollectionPos = collectible.getBasePosition();
		if (collectible.isCollected()) continue;

		Vector3 collectPos = collectible.getPosition();

		// Calculate distance (ignore Y for easier collection)
		float dx = playerPos.x - collectPos.x;
		float dz = playerPos.z - collectPos.z;
		float distance = std::sqrt(dx * dx + dz * dz);

		// INCREASED collision distance for easier collection
		float collisionDist = playerRadius + collectible.getRadius() + 0.7f; // Added 0.7f margin

		if (distance < collisionDist) {
			collectible.collect();
			m_collectedCount++;
			m_newCollection = true;
			m_lastCollectionPos = collectPos;
		}
	}
}