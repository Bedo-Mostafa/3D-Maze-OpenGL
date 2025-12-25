#ifndef MAZE_H
#define MAZE_H

#include "Mesh.h"
#include <vector>
#include "Vector3.h"

class Maze {
public:
	Maze(int width, int height);

	// Generates a proper DFS maze with a guaranteed path
	void generateMaze();

	void generateMeshes(Mesh& wallMesh, Mesh& floorMesh, Mesh& ceilingMesh);
	bool checkCollision(const Vector3& position, float radius) const;

	Vector3 getStartPosition() const;
	Vector3 getExitPosition() const; // New method

	int getWidth() const { return m_width; }
	int getHeight() const { return m_height; }
	void generateTeleportMesh(Mesh& mesh);

	int getCellType(int x, int z) const {
		if (x < 0 || x >= m_width || z < 0 || z >= m_height) return 1;
		return m_grid[z][x];
	}

private:
	int m_width;
	int m_height;
	std::vector<std::vector<int>> m_grid; // 1 = Wall, 0 = Path

	// Store grid coordinates (not world coordinates)
	std::pair<int, int> m_startCell;
	std::pair<int, int> m_endCell;

	// Helper for mesh generation
	void addWallQuad(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices,
		const Vector3& bottomLeft, const Vector3& bottomRight, float height, const Vector3& normal);
	void addFloor(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices,
		float x, float z, float cellSize);
	void addCeiling(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices,
		float x, float z, float cellSize, float ceilingHeight);
	void addTopFace(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices,
		float x, float z, float width, float height);
	void addBottomFace(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices,
		float x, float z, float width, float height);
};

#endif // MAZE_H