#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <memory>
#include "Renderer.h"
#include "Maze.h"
#include "Player.h"
#include "InputHandler.h"
#include "Mesh.h"
#include "Texture.h"
#include "ShaderProgram.h"
#include "GameUI.h"
#include "AudioManager.h"
#include "Collectible.h"
#include "FlashlightSystem.h"
#include "ScreenFlashEffect.h"  // NEW

class Game {
public:
    Game();
    bool initialize();
    void run();

private:
    // Initialization
    bool createWindow();
    bool initializeResources();

    // Game Loop
    void processInput();
    void update();
    void render();

    // Input Handling
    void handleMovement();
    void handleMouseLook();

    // Game Logic
    void checkWinCondition();
    void resetGame();

    // Rendering
    void renderCollectibles();

    // Utilities
    std::array<float, 16> createIdentityMatrix() const;
    std::array<float, 16> createCollectibleMatrix(const Vector3& position, float rotationY, float scale = 1.0f) const;
    void generateSphereMesh(Mesh& mesh, float radius, int segments, int rings);

    // Window & Rendering
    std::unique_ptr<sf::RenderWindow> m_window;
    std::unique_ptr<Renderer> m_renderer;

    // Game World
    std::unique_ptr<Maze> m_maze;
    std::unique_ptr<Player> m_player;

    // Meshes
    std::unique_ptr<Mesh> m_wallMesh;
    std::unique_ptr<Mesh> m_floorMesh;
    std::unique_ptr<Mesh> m_ceilingMesh;
    std::unique_ptr<Mesh> m_teleportMesh;
    std::unique_ptr<Mesh> m_collectibleMesh;

    // Textures
    std::unique_ptr<Texture> m_wallTexture;
    std::unique_ptr<Texture> m_floorTexture;
    std::unique_ptr<Texture> m_ceilingTexture;

    // Shaders
    std::unique_ptr<ShaderProgram> m_teleportShader;
    std::unique_ptr<ShaderProgram> m_collectibleShader;

    // Systems
    std::unique_ptr<InputHandler> m_inputHandler;
    std::unique_ptr<GameUI> m_ui;
    std::unique_ptr<AudioManager> m_audioManager;
    std::unique_ptr<CollectibleManager> m_collectibleManager;
    std::unique_ptr<FlashlightSystem> m_flashlight;
    std::unique_ptr<ScreenFlashEffect> m_screenFlash;  // NEW

    // Timing
    sf::Clock m_clock;
    float m_deltaTime;
    float m_totalTime;
    float m_walkTime;

    // Game State
    enum GameState { PLAYING, WON };
    GameState m_gameState = PLAYING;
    bool m_batteryWarningShown = false;
};