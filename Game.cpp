#include "Game.h"
#include <iostream>
#include <iomanip>
#include <cmath>

Game::Game() : m_deltaTime(0.0f), m_totalTime(0.0f), m_walkTime(0.0f) {}

bool Game::initialize() {
	if (!createWindow()) {
		return false;
	}

	if (!initializeResources()) {
		return false;
	}

	return true;
}

bool Game::createWindow() {
	sf::ContextSettings settings;
	settings.depthBits = 24;
	settings.stencilBits = 8;
	settings.antialiasingLevel = 4;
	settings.majorVersion = 3;
	settings.minorVersion = 3;
	settings.attributeFlags = sf::ContextSettings::Default;

	sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();

	m_window = std::make_unique<sf::RenderWindow>(
		desktopMode,
		"3D Maze Explorer - Ultimate Edition",
		sf::Style::Fullscreen,
		settings
	);

	if (!m_window) {
		std::cerr << "ERROR: Failed to create window" << std::endl;
		return false;
	}

	m_window->setActive(true);
	m_window->setVerticalSyncEnabled(true);
	m_window->setMouseCursorVisible(false);
	m_window->setMouseCursorGrabbed(true);

	return true;
}

bool Game::initializeResources() {
	m_renderer = std::make_unique<Renderer>();
	if (!m_renderer->initialize()) {
		std::cerr << "ERROR: Failed to initialize renderer" << std::endl;
		return false;
	}

	sf::Vector2u windowSize = m_window->getSize();
	m_renderer->setViewport(windowSize.x, windowSize.y);

	m_maze = std::make_unique<Maze>(15, 15);

	m_wallMesh = std::make_unique<Mesh>();
	m_floorMesh = std::make_unique<Mesh>();
	m_ceilingMesh = std::make_unique<Mesh>();
	m_teleportMesh = std::make_unique<Mesh>();

	m_maze->generateMeshes(*m_wallMesh, *m_floorMesh, *m_ceilingMesh);
	m_maze->generateTeleportMesh(*m_teleportMesh);

	if (!m_wallMesh->isValid() || !m_floorMesh->isValid() || !m_ceilingMesh->isValid()) {
		std::cerr << "ERROR: Mesh generation failed" << std::endl;
		return false;
	}

	m_wallTexture = std::make_unique<Texture>();
	m_floorTexture = std::make_unique<Texture>();
	m_ceilingTexture = std::make_unique<Texture>();

	if (!m_wallTexture->loadFromFile("textures/wall.png")) {
		std::cerr << "ERROR: Failed to load wall texture" << std::endl;
		return false;
	}

	if (!m_floorTexture->loadFromFile("textures/floor.png")) {
		std::cerr << "ERROR: Failed to load floor texture" << std::endl;
		return false;
	}

	if (!m_ceilingTexture->loadFromFile("textures/ceiling.png")) {
		std::cout << "  WARNING: ceiling.png not found, using floor.png" << std::endl;
		if (!m_ceilingTexture->loadFromFile("textures/floor.png")) {
			std::cerr << "ERROR: Failed to load ceiling fallback" << std::endl;
			return false;
		}
	}

	m_teleportShader = std::make_unique<ShaderProgram>();
	if (!m_teleportShader->loadFromFile("shaders/teleport_vertex.glsl", "shaders/teleport_fragment.glsl")) {
		std::cerr << "Failed to load teleport shaders!" << std::endl;
		return false;
	}

	m_collectibleShader = std::make_unique<ShaderProgram>();
	if (!m_collectibleShader->loadFromFile("shaders/collectible_vertex.glsl", "shaders/collectible_fragment.glsl")) {
		std::cerr << "WARNING: Failed to load collectible shaders" << std::endl;
	}

	m_collectibleMesh = std::make_unique<Mesh>();
	generateSphereMesh(*m_collectibleMesh, 0.4f, 16, 16);

	m_collectibleManager = std::make_unique<CollectibleManager>();
	m_collectibleManager->initialize(*m_maze);

	// === REMOVED: Particle system (replaced with screen flash) ===
	// m_particleSystem = std::make_unique<ParticleSystem>();

	// === NEW: Screen Flash Effect ===
	m_screenFlash = std::make_unique<ScreenFlashEffect>();
	std::cout << "Screen flash effect initialized" << std::endl;

	// === FLASHLIGHT SYSTEM ===
	m_flashlight = std::make_unique<FlashlightSystem>();
	m_flashlight->setDrainRate(0.02f);
	std::cout << "Flashlight system initialized (50 seconds battery)" << std::endl;

	m_audioManager = std::make_unique<AudioManager>();
	m_audioManager->initialize();

	m_audioManager->loadMusic("ambient", "audio/ambient.wav");
	m_audioManager->loadSound("collect", "audio/collect.wav");
	m_audioManager->loadSound("footstep", "audio/footstep.wav");
	m_audioManager->loadSound("win", "audio/win.wav");
	m_audioManager->playMusic("ambient", true, 30.0f);

	Vector3 startPos = m_maze->getStartPosition();
	m_player = std::make_unique<Player>(startPos);

	m_inputHandler = std::make_unique<InputHandler>();

	m_ui = std::make_unique<GameUI>();
	if (!m_ui->initialize(*m_maze)) {
		std::cerr << "WARNING: UI failed to initialize" << std::endl;
	}
	else {
		std::cout << "UI initialized successfully" << std::endl;
	}

	sf::Vector2u windowCenter = windowSize / 2u;
	m_inputHandler->initializeMousePosition(windowCenter.x, windowCenter.y);
	sf::Mouse::setPosition(sf::Vector2i(windowCenter.x, windowCenter.y), *m_window);

	return true;
}

void Game::run() {
	m_clock.restart();

	while (m_window->isOpen() && !m_inputHandler->shouldExit()) {
		m_deltaTime = m_clock.restart().asSeconds();

		processInput();
		update();
		render();
	}

	m_window->close();
}

void Game::processInput() {
	sf::Event event;
	while (m_window->pollEvent(event)) {
		m_inputHandler->processEvent(event);

		// Toggle flashlight with F key
		if (event.type == sf::Event::KeyPressed) {
			if (event.key.code == sf::Keyboard::F) {
				m_flashlight->toggle();
			}
		}

		if (m_gameState == WON) {
			if (event.type == sf::Event::MouseButtonPressed) {
				if (event.mouseButton.button == sf::Mouse::Left) {
					int action = m_ui->checkButtonPress(event.mouseButton.x, event.mouseButton.y);

					if (action == 1) {
						resetGame();
					}
					else if (action == 2) {
						m_window->close();
					}
				}
			}
		}
	}
}

void Game::update() {
	m_totalTime += m_deltaTime;

	if (m_gameState == PLAYING) {
		handleMouseLook();
		handleMovement();

		// Update flashlight battery
		m_flashlight->update(m_deltaTime);

		// Warn when battery is low
		if (m_flashlight->isLowBattery() && !m_batteryWarningShown) {
			std::cout << "WARNING: Flashlight battery low! ("
				<< static_cast<int>(m_flashlight->getBatteryPercent() * 100)
				<< "%)" << std::endl;
			m_batteryWarningShown = true;
		}

		// Reset warning when recharged
		if (m_flashlight->getBatteryPercent() > 0.5f) {
			m_batteryWarningShown = false;
		}

		// Update collectibles
		m_collectibleManager->update(m_deltaTime);
		m_collectibleManager->checkCollisions(
			m_player->getPosition(),
			m_player->getCollisionRadius()
		);

		// === TRIGGER SCREEN FLASH ON COLLECTION ===
		if (m_collectibleManager->hasNewCollection()) {
			m_audioManager->playSound("collect", 80.0f);

			// Trigger yellow flash (255, 220, 50) for 0.6 seconds
			m_screenFlash->trigger(sf::Color(255, 220, 50), 0.6f);
		}

		// Update minimap orb positions
		m_ui->updateCollectiblePositions(m_collectibleManager->getCollectibles());

		checkWinCondition();
		m_ui->update(m_deltaTime, *m_player, *m_maze);
	}

	// === NEW: UPDATE SCREEN FLASH ===
	m_screenFlash->update(m_deltaTime);

	m_audioManager->update();
	m_audioManager->updateListenerPosition(
		m_player->getPosition(),
		m_player->getCamera().getFront()
	);

	m_inputHandler->update();
}

void Game::handleMovement() {
	float forward = 0.0f;
	float right = 0.0f;

	if (m_inputHandler->isForwardPressed())  forward += 1.0f;
	if (m_inputHandler->isBackwardPressed()) forward -= 1.0f;
	if (m_inputHandler->isRightPressed())    right += 1.0f;
	if (m_inputHandler->isLeftPressed())     right -= 1.0f;

	float dt = m_deltaTime;
	if (dt > 0.1f) dt = 0.1f;

	bool isMoving = (forward != 0.0f || right != 0.0f);
	if (isMoving) {
		m_walkTime += dt;
		if (m_walkTime > 0.5f) {
			m_audioManager->playSound("footstep", 40.0f);
			m_walkTime = 0.0f;
		}
	}

	m_player->update(dt, forward, right, *m_maze);
}

void Game::handleMouseLook() {
	if (!m_inputHandler->isMouseCaptured()) {
		return;
	}

	sf::Vector2u size = m_window->getSize();
	sf::Vector2i center(size.x / 2, size.y / 2);
	sf::Vector2i currentPos = sf::Mouse::getPosition(*m_window);
	sf::Vector2i mouseDelta = currentPos - center;

	const float sensitivity = 0.1f;

	if (mouseDelta.x != 0 || mouseDelta.y != 0) {
		m_player->rotate(mouseDelta.x * sensitivity, -mouseDelta.y * sensitivity);
		sf::Mouse::setPosition(center, *m_window);
		m_inputHandler->initializeMousePosition(center.x, center.y);
	}
}

void Game::render() {
	// === 3D RENDERING ===
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);

	m_renderer->clear();

	// Pass flashlight state to renderer
	m_renderer->beginFrameWithFlashlight(
		m_player->getCamera(),
		m_maze->getExitPosition(),
		m_totalTime,
		m_flashlight->isOn(),
		m_flashlight->getIntensity(m_totalTime)
	);

	auto identityMatrix = createIdentityMatrix();

	m_renderer->renderMesh(*m_floorMesh, *m_floorTexture, identityMatrix.data());
	m_renderer->renderMesh(*m_wallMesh, *m_wallTexture, identityMatrix.data());
	m_renderer->renderMesh(*m_ceilingMesh, *m_ceilingTexture, identityMatrix.data());

	// Render collectibles
	renderCollectibles();

	// Render teleport portal
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDepthMask(GL_FALSE);

	m_teleportShader->use();

	Camera& cam = m_player->getCamera();
	auto view = cam.getViewMatrix();
	auto proj = cam.getProjectionMatrix(60.0f, m_renderer->getAspectRatio(), 0.1f, 100.0f);

	m_teleportShader->setUniformMatrix4fv("view", view.data());
	m_teleportShader->setUniformMatrix4fv("projection", proj.data());
	m_teleportShader->setUniformMatrix4fv("model", identityMatrix.data());
	m_teleportShader->setUniform("u_time", m_totalTime);

	Vector3 portalCenter = m_maze->getExitPosition();
	m_teleportShader->setUniform("u_portalCenter", portalCenter);

	m_teleportShader->setUniform("textureSampler", 0);

	m_teleportMesh->draw();

	// === RESET OPENGL STATE FOR UI ===
	glUseProgram(0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);

	// === 2D UI RENDERING ===
	m_window->pushGLStates();

	m_ui->draw(*m_window);
	m_ui->drawCollectibleCounter(
		*m_window,
		m_collectibleManager->getCollectedCount(),
		m_collectibleManager->getTotalCount()
	);

	// Draw battery indicator
	m_ui->drawBatteryIndicator(
		*m_window,
		m_flashlight->getBatteryPercent(),
		m_flashlight->isLowBattery()
	);

	// === NEW: DRAW SCREEN FLASH (must be last, on top of everything) ===
	m_screenFlash->draw(*m_window);

	if (m_gameState == WON) {
		m_ui->drawWinScreen(*m_window);
	}

	m_window->popGLStates();

	m_window->display();
}

void Game::renderCollectibles() {
	if (!m_collectibleShader || !m_collectibleMesh) return;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDepthMask(GL_FALSE);

	m_collectibleShader->use();

	Camera& cam = m_player->getCamera();
	auto view = cam.getViewMatrix();
	auto proj = cam.getProjectionMatrix(60.0f, m_renderer->getAspectRatio(), 0.1f, 100.0f);

	m_collectibleShader->setUniformMatrix4fv("view", view.data());
	m_collectibleShader->setUniformMatrix4fv("projection", proj.data());
	m_collectibleShader->setUniform("u_time", m_totalTime);

	for (const auto& collectible : m_collectibleManager->getCollectibles()) {
		if (collectible.isCollected()) continue;

		Vector3 pos = collectible.getPosition();
		float rotation = collectible.getRotation();
		float pulse = collectible.getPulse();

		auto modelMatrix = createCollectibleMatrix(pos, rotation, pulse);

		m_collectibleShader->setUniformMatrix4fv("model", modelMatrix.data());
		m_collectibleShader->setUniform("u_pulse", pulse);

		m_collectibleMesh->draw();
	}

	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
}

std::array<float, 16> Game::createIdentityMatrix() const {
	return {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
}

std::array<float, 16> Game::createCollectibleMatrix(const Vector3& position, float rotationY, float scale) const {
	float rad = rotationY * 3.14159 / 180.0f;
	float cosY = std::cos(rad);
	float sinY = std::sin(rad);

	return {
		scale * cosY, 0.0f, scale * sinY, 0.0f,
		0.0f, scale, 0.0f, 0.0f,
		scale * -sinY, 0.0f, scale * cosY, 0.0f,
		position.x, position.y, position.z, 1.0f
	};
}

void Game::checkWinCondition() {
	if (m_gameState == WON) return;

	Vector3 playerPos = m_player->getPosition();
	Vector3 exitPos = m_maze->getExitPosition();

	float dx = playerPos.x - exitPos.x;
	float dz = playerPos.z - exitPos.z;
	float distSq = dx * dx + dz * dz;

	if (distSq < (1.5f * 1.5f)) {
		m_gameState = WON;

		m_window->setMouseCursorVisible(true);
		m_window->setMouseCursorGrabbed(false);

		m_audioManager->playSound("win", 100.0f);

		std::cout << "*** MAZE SOLVED! ***" << std::endl;
		std::cout << "Collectibles: " << m_collectibleManager->getCollectedCount()
			<< "/" << m_collectibleManager->getTotalCount() << std::endl;
		std::cout << "Final Battery: " << static_cast<int>(m_flashlight->getBatteryPercent() * 100)
			<< "%" << std::endl;
	}
}

void Game::resetGame() {
	Vector3 startPos = m_maze->getStartPosition();
	m_player->setPosition(startPos);

	m_ui->resetTimer();
	m_collectibleManager->initialize(*m_maze);

	// Reset flashlight and screen flash
	m_flashlight = std::make_unique<FlashlightSystem>();
	m_flashlight->setDrainRate(0.02f);
	m_screenFlash = std::make_unique<ScreenFlashEffect>();
	m_batteryWarningShown = false;

	m_gameState = PLAYING;

	m_window->setMouseCursorVisible(false);
	m_window->setMouseCursorGrabbed(true);

	sf::Vector2u size = m_window->getSize();
	m_inputHandler->initializeMousePosition(size.x / 2, size.y / 2);
	sf::Mouse::setPosition(sf::Vector2i(size.x / 2, size.y / 2), *m_window);
}

void Game::generateSphereMesh(Mesh& mesh, float radius, int segments, int rings) {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	for (int ring = 0; ring <= rings; ++ring) {
		float phi = 3.14159 * ring / rings;
		float y = radius * std::cos(phi);
		float ringRadius = radius * std::sin(phi);

		for (int seg = 0; seg <= segments; ++seg) {
			float theta = 2.0f * 3.14159 * seg / segments;
			float x = ringRadius * std::cos(theta);
			float z = ringRadius * std::sin(theta);

			Vector3 pos(x, y, z);
			Vector3 normal = pos.normalize();

			float u = (float)seg / segments;
			float v = (float)ring / rings;

			vertices.emplace_back(pos, normal, u, v);
		}
	}

	for (int ring = 0; ring < rings; ++ring) {
		for (int seg = 0; seg < segments; ++seg) {
			int current = ring * (segments + 1) + seg;
			int next = current + segments + 1;

			indices.push_back(current);
			indices.push_back(next);
			indices.push_back(current + 1);

			indices.push_back(current + 1);
			indices.push_back(next);
			indices.push_back(next + 1);
		}
	}

	mesh.initialize(vertices, indices);
}