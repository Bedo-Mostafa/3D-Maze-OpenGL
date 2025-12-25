#ifndef GAMEUI_H
#define GAMEUI_H

#include <SFML/Graphics.hpp>
#include <vector>
#include "Maze.h"
#include "Player.h"
#include "Vector3.h"

// Forward declaration
class Collectible;

class GameUI {
public:
    GameUI();

    bool initialize(const Maze& maze);
    void update(float deltaTime, const Player& player, const Maze& maze);
    void draw(sf::RenderWindow& window);
    void drawWinScreen(sf::RenderWindow& window);
    void drawCollectibleCounter(sf::RenderWindow& window, int collected, int total);
    void drawBatteryIndicator(sf::RenderWindow& window, float batteryPercent, bool isFlickering);

    // Update collectible positions for minimap glow
    void updateCollectiblePositions(const std::vector<Collectible>& collectibles);

    int checkButtonPress(int mouseX, int mouseY);
    void resetTimer();

private:
    // Font
    sf::Font m_font;

    // Timer
    sf::Text m_timerText;
    float m_elapsedTime;

    // Minimap
    sf::RectangleShape m_minimapBackground;
    sf::RectangleShape m_wallShape;
    sf::RectangleShape m_playerDot;
    sf::RectangleShape m_exitMarker;

    std::vector<sf::Vector2i> m_wallGridCoords;
    sf::Vector2f m_playerGridPos;
    sf::Vector2i m_exitGridPos;
    sf::Vector2f m_minimapPosition;

    // NEW: Collectible positions for minimap glow
    std::vector<sf::Vector2f> m_collectibleGridPositions;

    float m_minimapScale;

    // Win screen
    sf::RectangleShape m_screenDimmer;
    sf::Text m_winText;
    sf::RectangleShape m_restartButton;
    sf::Text m_restartText;
    sf::RectangleShape m_exitButton;
    sf::Text m_exitText;

    // NEW: Battery indicator
    sf::RectangleShape m_batteryBackground;
    sf::RectangleShape m_batteryFill;
    sf::Text m_batteryText;
};

#endif // GAMEUI_H