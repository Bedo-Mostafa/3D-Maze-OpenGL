#include "Renderer.h"
#include <gl/glew/glew.h>
#include <iostream>

Renderer::Renderer() : m_viewportWidth(800), m_viewportHeight(600) {}

bool Renderer::initialize() {
	// Initialize GLEW
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		std::cerr << "GLEW initialization failed: " << glewGetErrorString(err) << std::endl;
		return false;
	}

	// Clear any GLEW initialization errors (this is normal)
	glGetError();

	initializeOpenGLState();

	// Load shader
	m_shader = std::make_unique<ShaderProgram>();
	if (!m_shader->loadFromFile("shaders/vertex.glsl", "shaders/fragment.glsl")) {
		std::cerr << "Failed to load shaders" << std::endl;
		return false;
	}

	return true;
}

void Renderer::initializeOpenGLState() {
	// Enable depth testing with correct function
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);  // Ensure depth buffer writing is enabled

	// Enable face culling to prevent rendering inside faces
	glEnable(GL_CULL_FACE);

	// Disable blending by default (enable only for transparent objects)
	glDisable(GL_BLEND);

	// Set clear color (dark blue-gray for better contrast)
	glClearColor(0.05f, 0.05f, 0.1f, 1.0f);

	// Set clear depth value
	glClearDepth(1.0);

	// Enable seamless cubemap filtering (if available)
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

void Renderer::setViewport(int width, int height) {
	m_viewportWidth = width;
	m_viewportHeight = height;
	glViewport(0, 0, width, height);
	std::cout << "Viewport set to: " << width << "x" << height << std::endl;
}

void Renderer::clear() {
	// Clear both color AND depth buffers every frame
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::beginFrame(const Camera& camera) {
	m_shader->use();

	// Set view and projection matrices
	auto viewMatrix = camera.getViewMatrix();

	// Use more appropriate near plane (0.1f instead of 0.05f)
	// This prevents z-fighting and improves depth precision
	auto projMatrix = camera.getProjectionMatrix(60.0f, getAspectRatio(), 0.1f, 100.0f);

	m_shader->setUniformMatrix4fv("view", viewMatrix.data());
	m_shader->setUniformMatrix4fv("projection", projMatrix.data());

	// Set lighting uniforms with better light position
	// Position light above and slightly in front of player
	Vector3 lightPos = camera.getPosition() + Vector3(0.0f, 5.0f, 5.0f);

	m_shader->setUniform("lightPos", lightPos);
	m_shader->setUniform("viewPos", camera.getPosition());
	m_shader->setUniform("lightColor", Vector3(1.0f, 1.0f, 1.0f));

	// Material properties - balanced for good visibility
	m_shader->setUniform("material.ambient", 0.4f);   // Increased for better visibility
	m_shader->setUniform("material.diffuse", 0.6f);   // Main lighting component
	m_shader->setUniform("material.specular", 0.3f);  // Reduced specular
	m_shader->setUniform("material.shininess", 16.0f); // Lower shininess for softer highlights

	// Set texture sampler
	m_shader->setUniform("textureSampler", 0);
}

void Renderer::renderMesh(const Mesh& mesh, const Texture& texture, const float* modelMatrix) {
	// Set model matrix
	m_shader->setUniformMatrix4fv("model", modelMatrix);

	// Bind texture
	texture.bind(0);

	// Draw mesh
	mesh.draw();
}

float Renderer::getAspectRatio() const {
	return static_cast<float>(m_viewportWidth) / static_cast<float>(m_viewportHeight);
}

void Renderer::beginFrameEnhanced(const Camera& camera, const Vector3& portalPos, float time) {
	m_shader->use();

	// Set view and projection matrices
	auto viewMatrix = camera.getViewMatrix();
	auto projMatrix = camera.getProjectionMatrix(60.0f, getAspectRatio(), 0.1f, 100.0f);

	m_shader->setUniformMatrix4fv("view", viewMatrix.data());
	m_shader->setUniformMatrix4fv("projection", projMatrix.data());

	// Basic lighting (overhead - very subtle)
	Vector3 lightPos = camera.getPosition() + Vector3(0.0f, 5.0f, 5.0f);
	m_shader->setUniform("lightPos", lightPos);
	m_shader->setUniform("viewPos", camera.getPosition());
	m_shader->setUniform("lightColor", Vector3(0.3f, 0.3f, 0.4f)); // Very dim blue-gray

	// Material properties (darker for dramatic effect)
	m_shader->setUniform("material.ambient", 0.15f);   // Very dark ambient
	m_shader->setUniform("material.diffuse", 0.3f);    // Low diffuse
	m_shader->setUniform("material.specular", 0.2f);
	m_shader->setUniform("material.shininess", 16.0f);

	// === FEATURE 3: FLASHLIGHT ===
	m_shader->setUniform("u_flashlightEnabled", true);
	m_shader->setUniform("u_flashlightPos", camera.getPosition());
	m_shader->setUniform("u_flashlightDir", camera.getFront());
	//m_shader->setUniform("u_flashlightCutoff", std::cos(25.0f * 3.14159 / 180.0f));      // 25 degree inner
	//m_shader->setUniform("u_flashlightOuterCutoff", std::cos(35.0f * 3.14159 / 180.0f)); // 35 degree outer
	m_shader->setUniform("u_flashlightIntensity", 1.5f); // Bright flashlight

	// === FEATURE 3: FOG ===
	m_shader->setUniform("u_fogEnabled", true);
	m_shader->setUniform("u_fogColor", Vector3(0.02f, 0.03f, 0.05f)); // Very dark blue
	m_shader->setUniform("u_fogDensity", 0.15f);
	m_shader->setUniform("u_fogStart", 5.0f);  // Fog starts at 5 units
	m_shader->setUniform("u_fogEnd", 15.0f);   // Completely dark at 15 units

	// === FEATURE 3: PORTAL LIGHT ===
	m_shader->setUniform("u_portalLightEnabled", true);
	m_shader->setUniform("u_portalPos", portalPos);
	m_shader->setUniform("u_portalColor", Vector3(0.0f, 0.8f, 1.0f)); // Cyan
	m_shader->setUniform("u_portalRadius", 8.0f); // Lights walls within 8 units

	// Pulsing effect for portal light
	float portalPulse = 0.8f + 0.2f * std::sin(time * 1.5f);
	Vector3 portalColorPulsed = Vector3(0.0f, 0.8f * portalPulse, 1.0f * portalPulse);
	m_shader->setUniform("u_portalColor", portalColorPulsed);

	// Set texture sampler
	m_shader->setUniform("textureSampler", 0);
}

// Add this method to Renderer.cpp

void Renderer::beginFrameWithFlashlight(const Camera& camera, const Vector3& portalPos,
	float time, bool flashlightOn, float flashlightIntensity) {
	m_shader->use();

	auto viewMatrix = camera.getViewMatrix();
	auto projMatrix = camera.getProjectionMatrix(60.0f, getAspectRatio(), 0.1f, 100.0f);

	m_shader->setUniformMatrix4fv("view", viewMatrix.data());
	m_shader->setUniformMatrix4fv("projection", projMatrix.data());

	Vector3 lightPos = camera.getPosition() + Vector3(0.0f, 5.0f, 5.0f);
	m_shader->setUniform("lightPos", lightPos);
	m_shader->setUniform("viewPos", camera.getPosition());
	m_shader->setUniform("lightColor", Vector3(0.3f, 0.3f, 0.4f));

	m_shader->setUniform("material.ambient", 0.15f);
	m_shader->setUniform("material.diffuse", 0.3f);
	m_shader->setUniform("material.specular", 0.2f);
	m_shader->setUniform("material.shininess", 16.0f);

	// === FLASHLIGHT WITH BATTERY CONTROL ===
	m_shader->setUniform("u_flashlightEnabled", flashlightOn);
	m_shader->setUniform("u_flashlightPos", camera.getPosition());
	m_shader->setUniform("u_flashlightDir", camera.getFront());
	m_shader->setUniform("u_flashlightIntensity", flashlightIntensity); // Dynamic intensity

	// Fog
	m_shader->setUniform("u_fogEnabled", true);
	m_shader->setUniform("u_fogColor", Vector3(0.02f, 0.03f, 0.05f));
	m_shader->setUniform("u_fogDensity", 0.15f);
	m_shader->setUniform("u_fogStart", 5.0f);
	m_shader->setUniform("u_fogEnd", 15.0f);

	// Portal light
	m_shader->setUniform("u_portalLightEnabled", true);
	m_shader->setUniform("u_portalPos", portalPos);

	float portalPulse = 0.8f + 0.2f * std::sin(time * 1.5f);
	Vector3 portalColorPulsed = Vector3(0.0f, 0.8f * portalPulse, 1.0f * portalPulse);
	m_shader->setUniform("u_portalColor", portalColorPulsed);
	m_shader->setUniform("u_portalRadius", 8.0f);

	m_shader->setUniform("textureSampler", 0);
}

// Keep old method for compatibility
//void Renderer::beginFrame(const Camera& camera) {
//	beginFrameWithFlashlight(camera, Vector3(0, 0, 0), 0.0f, true, 1.5f);
//}