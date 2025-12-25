#ifndef MESH_H
#define MESH_H

#include <vector>
#include <gl/glew/glew.h>
#include "Vector3.h"

/**
 * @struct Vertex
 * @brief Contains all vertex attributes
 */
struct Vertex {
    Vector3 position;
    Vector3 normal;
    float texCoordX;
    float texCoordY;

    Vertex(const Vector3& pos, const Vector3& norm, float u, float v)
        : position(pos), normal(norm), texCoordX(u), texCoordY(v) {}
};

/**
 * @class Mesh
 * @brief Manages 3D mesh data with vertices, normals, and texture coordinates
 * 
 * Single Responsibility: Handles mesh data and OpenGL buffer management
 */
class Mesh {
public:
    Mesh();
    ~Mesh();

    // Disable copy operations
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    // Enable move operations
    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;

    /**
     * @brief Initialize mesh with vertex and index data
     * @param vertices Vertex data
     * @param indices Index data for element drawing
     */
    void initialize(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);

    /**
     * @brief Draw the mesh
     */
    void draw() const;

    /**
     * @brief Check if mesh is valid
     */
    bool isValid() const { return m_vao != 0; }

private:
    GLuint m_vao;  // Vertex Array Object
    GLuint m_vbo;  // Vertex Buffer Object
    GLuint m_ebo;  // Element Buffer Object
    unsigned int m_indexCount;

    /**
     * @brief Clean up OpenGL resources
     */
    void cleanup();
};

#endif // MESH_H