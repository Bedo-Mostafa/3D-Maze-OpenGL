#include "Maze.h"
#include <cmath>
#include <algorithm>
#include<stack>
#include <iostream>

Maze::Maze(int width, int height) : m_width(width), m_height(height) {
	// Ensure dimensions are odd for this algorithm to work best
	if (m_width % 2 == 0) m_width++;
	if (m_height % 2 == 0) m_height++;

	m_grid.resize(m_height, std::vector<int>(m_width, 1)); // Fill with walls (1)

	// Seed random
	std::srand(static_cast<unsigned int>(std::time(nullptr)));

	generateMaze();
}

void Maze::generateMaze() {
	// 1. Reset grid to all walls
	for (int i = 0; i < m_height; ++i) {
		std::fill(m_grid[i].begin(), m_grid[i].end(), 1);
	}

	// 2. Define Start and End
	m_startCell = { 1, 1 };
	m_endCell = { m_width - 2, m_height - 2 };

	// 3. Recursive Backtracker Algorithm
	std::stack<std::pair<int, int>> stack;
	m_grid[m_startCell.second][m_startCell.first] = 0;
	stack.push(m_startCell);

	const int dirs[4][2] = { {0, -2}, {0, 2}, {-2, 0}, {2, 0} };

	while (!stack.empty()) {
		std::pair<int, int> current = stack.top();
		int x = current.first;
		int y = current.second;
		std::vector<int> neighbors;

		for (int i = 0; i < 4; ++i) {
			int nx = x + dirs[i][0];
			int ny = y + dirs[i][1];
			if (nx > 0 && nx < m_width - 1 && ny > 0 && ny < m_height - 1) {
				if (m_grid[ny][nx] == 1) neighbors.push_back(i);
			}
		}

		if (!neighbors.empty()) {
			int nextDir = neighbors[std::rand() % neighbors.size()];
			int nx = x + dirs[nextDir][0];
			int ny = y + dirs[nextDir][1];
			m_grid[y + dirs[nextDir][1] / 2][x + dirs[nextDir][0] / 2] = 0;
			m_grid[ny][nx] = 0;
			stack.push({ nx, ny });
		}
		else {
			stack.pop();
		}
	}

	// preventing the "trapped inside walls" feeling at spawn.
	if (m_height > 2 && m_width > 2) {
		m_grid[1][2] = 0; // Clear cell to the South
		m_grid[2][1] = 0; // Clear cell to the East
		m_grid[2][2] = 0; // Clear diagonal for extra space
	}

	// Ensure Exit is open
	m_grid[m_endCell.second][m_endCell.first] = 0;
}

void Maze::generateMeshes(Mesh& wallMesh, Mesh& floorMesh, Mesh& ceilingMesh) {
	std::vector<Vertex> wallVertices;
	std::vector<unsigned int> wallIndices;
	std::vector<Vertex> floorVertices;
	std::vector<unsigned int> floorIndices;
	std::vector<Vertex> ceilingVertices;
	std::vector<unsigned int> ceilingIndices;

	const float cellSize = 2.0f;
	const float wallHeight = 3.0f;

	for (int i = 0; i < m_height; ++i) {
		for (int j = 0; j < m_width; ++j) {
			float x = j * cellSize;
			float z = i * cellSize;

			// Coordinates for the 4 corners of the cell
			Vector3 c00(x, 0, z);              // Top-Left
			Vector3 c10(x + cellSize, 0, z);   // Top-Right
			Vector3 c01(x, 0, z + cellSize);   // Bottom-Left
			Vector3 c11(x + cellSize, 0, z + cellSize); // Bottom-Right

			if (m_grid[i][j] == 1) {
				// Wall Cell

				// NORTH WALL (Face -Z)
				// Generated Left-to-Right (c00 to c10)
				if (i == 0 || m_grid[i - 1][j] == 0)
					addWallQuad(wallVertices, wallIndices, c10, c00, wallHeight, Vector3(0, 0, -1));

				// SOUTH WALL (Face +Z)
				// Generated Left-to-Right (c01 to c11)
				if (i == m_height - 1 || m_grid[i + 1][j] == 0)
					addWallQuad(wallVertices, wallIndices, c01, c11, wallHeight, Vector3(0, 0, 1));

				// WEST WALL (Face -X)
				// Generated Left-to-Right (c01 to c00)
				if (j == 0 || m_grid[i][j - 1] == 0)
					addWallQuad(wallVertices, wallIndices, c00, c01, wallHeight, Vector3(-1, 0, 0));

				// EAST WALL (Face +X)
				// Generated Left-to-Right (c11 to c10)
				if (j == m_width - 1 || m_grid[i][j + 1] == 0)
					addWallQuad(wallVertices, wallIndices, c11, c10, wallHeight, Vector3(1, 0, 0));

				addTopFace(wallVertices, wallIndices, x, z, cellSize, wallHeight);
			}
			else {
				// Empty Cell
				addFloor(floorVertices, floorIndices, x, z, cellSize);
				addCeiling(ceilingVertices, ceilingIndices, x, z, cellSize, wallHeight);
			}
		}
	}

	wallMesh.initialize(wallVertices, wallIndices);
	floorMesh.initialize(floorVertices, floorIndices);
	ceilingMesh.initialize(ceilingVertices, ceilingIndices);
}

void Maze::generateTeleportMesh(Mesh& mesh) {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	const float height = 2.5f;
	const float cylinderRadius = 0.8f;
	const float platformRadius = 1.2f;  // Ground platform
	const int segments = 32; // Higher for smoother appearance

	// Get Exit Center
	float cellSize = 2.0f;
	float cx = (m_endCell.first + 0.5f) * cellSize;
	float cz = (m_endCell.second + 0.5f) * cellSize;

	for (int i = 0; i <= segments; ++i) {
		float theta = (float)i / segments * 2.0f * 3.14159f;
		float x = cos(theta) * cylinderRadius;
		float z = sin(theta) * cylinderRadius;
		float u = (float)i / segments;

		Vector3 normal(x, 0.0f, z);
		normal = normal.normalize();

		// Bottom Vertex (y = 0)
		vertices.emplace_back(
			Vector3(cx + x, 0.0f, cz + z),
			normal,
			u, 1.0f
		);

		// Top Vertex (y = height)
		vertices.emplace_back(
			Vector3(cx + x, height, cz + z),
			normal,
			u, 0.0f
		);
	}

	// Generate Indices for Cylinder
	for (int i = 0; i < segments; ++i) {
		int bottom1 = i * 2;
		int top1 = bottom1 + 1;
		int bottom2 = (i + 1) * 2;
		int top2 = bottom2 + 1;

		// Triangle 1
		indices.push_back(bottom1);
		indices.push_back(top1);
		indices.push_back(bottom2);

		// Triangle 2
		indices.push_back(top1);
		indices.push_back(top2);
		indices.push_back(bottom2);
	}

	unsigned int centerIndex = static_cast<unsigned int>(vertices.size());

	// Center vertex at ground level
	vertices.emplace_back(
		Vector3(cx, 0.01f, cz),  // Slightly above ground to prevent z-fighting
		Vector3(0.0f, 1.0f, 0.0f),
		0.5f, 0.5f
	);

	// Create radial platform
	int platformSegments = 24;
	unsigned int platformStartIndex = static_cast<unsigned int>(vertices.size());

	for (int i = 0; i <= platformSegments; ++i) {
		float theta = (float)i / platformSegments * 2.0f * 3.14159f;
		float x = cos(theta) * platformRadius;
		float z = sin(theta) * platformRadius;

		// UV coordinates radiate from center
		float u = 0.5f + 0.5f * cos(theta);
		float v = 0.5f + 0.5f * sin(theta);

		vertices.emplace_back(
			Vector3(cx + x, 0.01f, cz + z),
			Vector3(0.0f, 1.0f, 0.0f),
			u, v
		);
	}

	// Create triangles for platform
	for (int i = 0; i < platformSegments; ++i) {
		indices.push_back(centerIndex);
		indices.push_back(platformStartIndex + i);
		indices.push_back(platformStartIndex + i + 1);
	}

	unsigned int topCenterIndex = static_cast<unsigned int>(vertices.size());

	// Top center point
	vertices.emplace_back(
		Vector3(cx, height, cz),
		Vector3(0.0f, 1.0f, 0.0f),
		0.5f, 0.5f
	);

	// Top ring
	unsigned int topRingStart = static_cast<unsigned int>(vertices.size());
	float topCapRadius = cylinderRadius * 0.3f; // Smaller at top

	for (int i = 0; i <= segments / 2; ++i) {
		float theta = (float)i / (segments / 2) * 2.0f * 3.14159f;
		float x = cos(theta) * topCapRadius;
		float z = sin(theta) * topCapRadius;

		float u = 0.5f + 0.5f * cos(theta);
		float v = 0.5f + 0.5f * sin(theta);

		vertices.emplace_back(
			Vector3(cx + x, height, cz + z),
			Vector3(0.0f, 1.0f, 0.0f),
			u, v
		);
	}

	// Create triangles for top cap
	for (int i = 0; i < segments / 2; ++i) {
		indices.push_back(topCenterIndex);
		indices.push_back(topRingStart + i);
		indices.push_back(topRingStart + i + 1);
	}

	mesh.initialize(vertices, indices);
}

void Maze::addWallQuad(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices,
	const Vector3& bottomLeft, const Vector3& bottomRight, float height, const Vector3& normal) {

	unsigned int baseIndex = static_cast<unsigned int>(vertices.size());
	Vector3 up(0, 1, 0);

	// 4 Corners of the wall face
	Vector3 v0 = bottomLeft;
	Vector3 v1 = bottomRight;
	Vector3 v2 = bottomRight + (up * height);
	Vector3 v3 = bottomLeft + (up * height);

	float texRepeat = 1.0f;

	// Add Vertices (Standard UV mapping)
	vertices.emplace_back(v0, normal, 0.0f, 0.0f);
	vertices.emplace_back(v1, normal, texRepeat, 0.0f);
	vertices.emplace_back(v2, normal, texRepeat, texRepeat);
	vertices.emplace_back(v3, normal, 0.0f, texRepeat);

	// Add Indices (Counter-Clockwise 0-1-2, 0-2-3)
	indices.push_back(baseIndex + 0);
	indices.push_back(baseIndex + 1);
	indices.push_back(baseIndex + 2);
	indices.push_back(baseIndex + 0);
	indices.push_back(baseIndex + 2);
	indices.push_back(baseIndex + 3);
}

void Maze::addFloor(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices,
	float x, float z, float cellSize) {
	unsigned int baseIndex = static_cast<unsigned int>(vertices.size());
	Vector3 normal(0, 1, 0);  // Normal points UP

	// Create floor quad at Y = 0
	vertices.emplace_back(Vector3(x, 0, z), normal, 0.0f, 0.0f);
	vertices.emplace_back(Vector3(x + cellSize, 0, z), normal, 1.0f, 0.0f);
	vertices.emplace_back(Vector3(x + cellSize, 0, z + cellSize), normal, 1.0f, 1.0f);
	vertices.emplace_back(Vector3(x, 0, z + cellSize), normal, 0.0f, 1.0f);

	// Add indices (CCW when viewed from above)
	indices.push_back(baseIndex + 0);
	indices.push_back(baseIndex + 1);
	indices.push_back(baseIndex + 2);
	indices.push_back(baseIndex + 0);
	indices.push_back(baseIndex + 2);
	indices.push_back(baseIndex + 3);
}

void Maze::addCeiling(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices,
	float x, float z, float cellSize, float ceilingHeight) {
	unsigned int baseIndex = static_cast<unsigned int>(vertices.size());
	Vector3 normal(0, -1, 0);  // Normal points DOWN

	// Create ceiling quad at Y = ceilingHeight
	vertices.emplace_back(Vector3(x, ceilingHeight, z), normal, 0.0f, 0.0f);
	vertices.emplace_back(Vector3(x, ceilingHeight, z + cellSize), normal, 0.0f, 1.0f);
	vertices.emplace_back(Vector3(x + cellSize, ceilingHeight, z + cellSize), normal, 1.0f, 1.0f);
	vertices.emplace_back(Vector3(x + cellSize, ceilingHeight, z), normal, 1.0f, 0.0f);

	// Add indices (CCW when viewed from below)
	indices.push_back(baseIndex + 0);
	indices.push_back(baseIndex + 1);
	indices.push_back(baseIndex + 2);
	indices.push_back(baseIndex + 0);
	indices.push_back(baseIndex + 2);
	indices.push_back(baseIndex + 3);
}

void Maze::addTopFace(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices,
	float x, float z, float width, float height) {
	unsigned int baseIndex = static_cast<unsigned int>(vertices.size());
	Vector3 normal(0.0f, 1.0f, 0.0f);

	// Top Face (viewed from above)
	vertices.emplace_back(Vector3(x, height, z), normal, 0.0f, 0.0f);
	vertices.emplace_back(Vector3(x + width, height, z), normal, 1.0f, 0.0f);
	vertices.emplace_back(Vector3(x + width, height, z + width), normal, 1.0f, 1.0f);
	vertices.emplace_back(Vector3(x, height, z + width), normal, 0.0f, 1.0f);

	// Standard CCW Winding
	indices.push_back(baseIndex + 0);
	indices.push_back(baseIndex + 1);
	indices.push_back(baseIndex + 2);
	indices.push_back(baseIndex + 0);
	indices.push_back(baseIndex + 2);
	indices.push_back(baseIndex + 3);
}

void Maze::addBottomFace(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices,
	float x, float z, float width, float height) {
	unsigned int baseIndex = static_cast<unsigned int>(vertices.size());
	Vector3 normal(0.0f, -1.0f, 0.0f);

	// Bottom Face
	vertices.emplace_back(Vector3(x, 0.0f, z), normal, 0.0f, 0.0f);
	vertices.emplace_back(Vector3(x, 0.0f, z + width), normal, 0.0f, 1.0f);
	vertices.emplace_back(Vector3(x + width, 0.0f, z + width), normal, 1.0f, 1.0f);
	vertices.emplace_back(Vector3(x + width, 0.0f, z), normal, 1.0f, 0.0f);

	// Standard CCW Winding
	indices.push_back(baseIndex + 0);
	indices.push_back(baseIndex + 1);
	indices.push_back(baseIndex + 2);
	indices.push_back(baseIndex + 0);
	indices.push_back(baseIndex + 2);
	indices.push_back(baseIndex + 3);
}

bool Maze::checkCollision(const Vector3& position, float radius) const {
	const float cellSize = 2.0f;

	// Simple bounds check with margin
	if (position.x < radius || position.z < radius) {
		return true;
	}

	float maxX = (m_width * cellSize) - radius;
	float maxZ = (m_height * cellSize) - radius;
	if (position.x > maxX || position.z > maxZ) {
		return true;
	}

	// Convert to grid with clamping
	int gridX = static_cast<int>(position.x / cellSize);
	int gridZ = static_cast<int>(position.z / cellSize);

	if (gridX < 0) gridX = 0;
	if (gridX >= m_width) gridX = m_width - 1;
	if (gridZ < 0) gridZ = 0;
	if (gridZ >= m_height) gridZ = m_height - 1;

	// Check 3x3 grid around player
	for (int dz = -1; dz <= 1; ++dz) {
		for (int dx = -1; dx <= 1; ++dx) {
			int checkX = gridX + dx;
			int checkZ = gridZ + dz;

			// Skip out of bounds
			if (checkX < 0 || checkX >= m_width ||
				checkZ < 0 || checkZ >= m_height) {
				continue;
			}

			// Check wall collision using AABB
			if (m_grid[checkZ][checkX] == 1) {
				// Wall boundaries
				float wallMinX = checkX * cellSize;
				float wallMaxX = (checkX + 1) * cellSize;
				float wallMinZ = checkZ * cellSize;
				float wallMaxZ = (checkZ + 1) * cellSize;

				// Closest point on wall to player
				float closestX = position.x;
				if (closestX < wallMinX) closestX = wallMinX;
				if (closestX > wallMaxX) closestX = wallMaxX;

				float closestZ = position.z;
				if (closestZ < wallMinZ) closestZ = wallMinZ;
				if (closestZ > wallMaxZ) closestZ = wallMaxZ;

				// Distance check
				float dx = position.x - closestX;
				float dz = position.z - closestZ;
				float distanceSquared = dx * dx + dz * dz;

				if (distanceSquared < (radius * radius)) {
					return true;
				}
			}
		}
	}

	return false;
}

Vector3 Maze::getStartPosition() const {
	const float cellSize = 2.0f;
	// Center of the start cell
	float centerX = (m_startCell.first + 0.5f) * cellSize;
	float centerZ = (m_startCell.second + 0.5f) * cellSize;
	return Vector3(centerX, 0.0f, centerZ);
}

Vector3 Maze::getExitPosition() const {
	const float cellSize = 2.0f;
	// Center of the end cell
	float centerX = (m_endCell.first + 0.5f) * cellSize;
	float centerZ = (m_endCell.second + 0.5f) * cellSize;
	return Vector3(centerX, 0.0f, centerZ);
}