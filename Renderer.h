#ifndef RENDERER_H
#define RENDERER_H

#include <memory>
#include "ShaderProgram.h"
#include "Camera.h"
#include "Mesh.h"
#include "Texture.h"

/**
 * @class Renderer
 * @brief Manages OpenGL rendering operations
 *
 * Single Responsibility: Handles rendering logic
 * Dependency Inversion: Depends on abstractions (Mesh, Texture, etc.)
 */
class Renderer {
public:
	Renderer();
	~Renderer() = default;

	/**
	 * @brief Initialize OpenGL state and load shaders
	 * @return true if successful, false otherwise
	 */
	bool initialize();

	/**
	 * @brief Set viewport size
	 */
	void setViewport(int width, int height);

	/**
	 * @brief Clear the screen
	 */
	void clear();

	void beginFrameEnhanced(const Camera& camera, const Vector3& portalPos, float time);

	void beginFrameWithFlashlight(const Camera& camera, const Vector3& portalPos, float time, bool flashlightOn, float flashlightIntensity);

	/**
	 * @brief Begin rendering frame
	 * @param camera Camera to use for rendering
	 */
	void beginFrame(const Camera& camera);

	/**
	 * @brief Render a mesh with a texture
	 * @param mesh Mesh to render
	 * @param texture Texture to apply
	 * @param modelMatrix Model transformation matrix
	 */
	void renderMesh(const Mesh& mesh, const Texture& texture, const float* modelMatrix);

	/**
	 * @brief Get aspect ratio
	 */
	float getAspectRatio() const;

private:
	std::unique_ptr<ShaderProgram> m_shader;
	int m_viewportWidth;
	int m_viewportHeight;

	/**
	 * @brief Initialize OpenGL settings
	 */
	void initializeOpenGLState();
};

#endif // RENDERER_H