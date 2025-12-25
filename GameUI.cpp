#include "GameUI.h"
#include "Collectible.h"
#include <iomanip>
#include <sstream>
#include <iostream>
#include <cmath>

GameUI::GameUI() : m_elapsedTime(0.0f), m_minimapScale(10.0f) {}

bool GameUI::initialize(const Maze& maze) {
    // 1. Load Font
    if (!m_font.loadFromFile("RubikMaze-Regular.ttf")) {
        if (!m_font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
            std::cerr << "WARNING: Failed to load font" << std::endl;
            return false;
        }
    }

    // 2. Setup Timer
    m_timerText.setFont(m_font);
    m_timerText.setCharacterSize(28);
    m_timerText.setFillColor(sf::Color::White);
    m_timerText.setOutlineColor(sf::Color::Black);
    m_timerText.setOutlineThickness(2.0f);
    m_timerText.setPosition(20.0f, 20.0f);

    // 3. Setup Minimap
    int mazeW = maze.getWidth();
    int mazeH = maze.getHeight();
    float mapWidth = mazeW * m_minimapScale;
    float mapHeight = mazeH * m_minimapScale;

    m_minimapBackground.setSize(sf::Vector2f(mapWidth, mapHeight));
    m_minimapBackground.setFillColor(sf::Color(0, 0, 0, 180));
    m_minimapBackground.setOutlineColor(sf::Color(0, 200, 255));
    m_minimapBackground.setOutlineThickness(2.0f);

    m_wallShape.setSize(sf::Vector2f(m_minimapScale, m_minimapScale));
    m_wallShape.setFillColor(sf::Color(100, 100, 100));

    m_wallGridCoords.clear();
    for (int y = 0; y < mazeH; ++y) {
        for (int x = 0; x < mazeW; ++x) {
            if (maze.getCellType(x, y) == 1) {
                m_wallGridCoords.push_back(sf::Vector2i(x, y));
            }
        }
    }

    m_playerDot.setSize(sf::Vector2f(m_minimapScale * 0.8f, m_minimapScale * 0.8f));
    m_playerDot.setFillColor(sf::Color::Red);

    m_exitMarker.setSize(sf::Vector2f(m_minimapScale, m_minimapScale));
    m_exitMarker.setFillColor(sf::Color::Green);

    Vector3 exitWorldPos = maze.getExitPosition();
    m_exitGridPos.x = static_cast<int>(exitWorldPos.x / 2.0f);
    m_exitGridPos.y = static_cast<int>(exitWorldPos.z / 2.0f);

    // 4. Setup Win Screen
    m_screenDimmer.setFillColor(sf::Color(0, 0, 0, 200));

    m_winText.setFont(m_font);
    m_winText.setString("YOU ESCAPED!");
    m_winText.setCharacterSize(60);
    m_winText.setFillColor(sf::Color::Green);
    m_winText.setOutlineColor(sf::Color::Black);
    m_winText.setOutlineThickness(3.0f);

    m_restartButton.setSize(sf::Vector2f(200, 50));
    m_restartButton.setFillColor(sf::Color(50, 50, 50));
    m_restartButton.setOutlineColor(sf::Color::White);
    m_restartButton.setOutlineThickness(2);

    m_restartText.setFont(m_font);
    m_restartText.setString("RESTART");
    m_restartText.setCharacterSize(30);
    m_restartText.setFillColor(sf::Color::White);

    m_exitButton.setSize(sf::Vector2f(200, 50));
    m_exitButton.setFillColor(sf::Color(50, 50, 50));
    m_exitButton.setOutlineColor(sf::Color::White);
    m_exitButton.setOutlineThickness(2);

    m_exitText.setFont(m_font);
    m_exitText.setString("EXIT");
    m_exitText.setCharacterSize(30);
    m_exitText.setFillColor(sf::Color::White);

    // 5. Setup Battery Indicator
    m_batteryBackground.setSize(sf::Vector2f(120, 30));
    m_batteryBackground.setFillColor(sf::Color(30, 30, 30, 200));
    m_batteryBackground.setOutlineColor(sf::Color(80, 80, 80));
    m_batteryBackground.setOutlineThickness(2.0f);

    m_batteryFill.setSize(sf::Vector2f(110, 20));
    m_batteryFill.setFillColor(sf::Color::Green);

    m_batteryText.setFont(m_font);
    m_batteryText.setCharacterSize(18);
    m_batteryText.setFillColor(sf::Color::White);
    m_batteryText.setOutlineColor(sf::Color::Black);
    m_batteryText.setOutlineThickness(1.5f);

    return true;
}

void GameUI::update(float deltaTime, const Player& player, const Maze& maze) {
    m_elapsedTime += deltaTime;

    int minutes = static_cast<int>(m_elapsedTime) / 60;
    int seconds = static_cast<int>(m_elapsedTime) % 60;
    int milliseconds = static_cast<int>((m_elapsedTime - static_cast<int>(m_elapsedTime)) * 100);

    std::stringstream ss;
    ss << "Time: " << std::setfill('0') << std::setw(2) << minutes << ":"
        << std::setw(2) << seconds << "." << std::setw(2) << milliseconds;
    m_timerText.setString(ss.str());

    Vector3 pos = player.getPosition();
    m_playerGridPos.x = pos.x / 2.0f;
    m_playerGridPos.y = pos.z / 2.0f;
}

void GameUI::resetTimer() {
    m_elapsedTime = 0.0f;
}

void GameUI::updateCollectiblePositions(const std::vector<class Collectible>& collectibles) {
    m_collectibleGridPositions.clear();

    for (const auto& collectible : collectibles) {
        if (collectible.isCollected()) continue;

        // Use BASE position (not animated position with hover offset)
        Vector3 basePos = collectible.getBasePosition();
        sf::Vector2f gridPos;
        gridPos.x = basePos.x / 2.0f;
        gridPos.y = basePos.z / 2.0f;

        m_collectibleGridPositions.push_back(gridPos);
    }
}

void GameUI::draw(sf::RenderWindow& window) {
    sf::Vector2u windowSize = window.getSize();
    float mapWidth = m_minimapBackground.getSize().x;
    float mapHeight = m_minimapBackground.getSize().y;

    float startX = windowSize.x - mapWidth - 20.0f;
    float startY = 20.0f;

    m_minimapPosition = sf::Vector2f(startX, startY);
    m_minimapBackground.setPosition(m_minimapPosition);

    // Draw minimap background
    window.draw(m_minimapBackground);

    // Draw walls
    for (const auto& coord : m_wallGridCoords) {
        float px = startX + (coord.x * m_minimapScale);
        float py = startY + (coord.y * m_minimapScale);
        m_wallShape.setPosition(px, py);
        window.draw(m_wallShape);
    }

    // === FIXED: DRAW COLLECTIBLE ORBS WITH GLOW (using base positions) ===
    for (const auto& gridPos : m_collectibleGridPositions) {
        float orbX = startX + (gridPos.x * m_minimapScale);
        float orbY = startY + (gridPos.y * m_minimapScale);

        // Glow effect (larger circle, semi-transparent)
        sf::CircleShape glow(m_minimapScale * 0.8f);
        glow.setPosition(orbX - m_minimapScale * 0.3f, orbY - m_minimapScale * 0.3f);
        glow.setFillColor(sf::Color(255, 150, 0, 80)); // Orange glow
        window.draw(glow);

        // Orb itself
        sf::CircleShape orb(m_minimapScale * 0.4f);
        orb.setPosition(orbX, orbY);
        orb.setFillColor(sf::Color(255, 180, 0)); // Bright orange
        orb.setOutlineColor(sf::Color(255, 220, 100));
        orb.setOutlineThickness(1.0f);
        window.draw(orb);
    }

    // Draw exit marker
    float exitScreenX = startX + (m_exitGridPos.x * m_minimapScale);
    float exitScreenY = startY + (m_exitGridPos.y * m_minimapScale);
    m_exitMarker.setPosition(exitScreenX, exitScreenY);
    window.draw(m_exitMarker);

    // Draw player
    float playerScreenX = startX + (m_playerGridPos.x * m_minimapScale);
    float playerScreenY = startY + (m_playerGridPos.y * m_minimapScale);
    m_playerDot.setPosition(playerScreenX, playerScreenY);
    window.draw(m_playerDot);

    // Draw timer
    window.draw(m_timerText);
}

void GameUI::drawBatteryIndicator(sf::RenderWindow& window, float batteryPercent, bool isFlickering) {
    sf::Vector2u windowSize = window.getSize();

    // Position: Bottom-left corner
    float xPos = 20.0f;
    float yPos = windowSize.y - 60.0f;

    m_batteryBackground.setPosition(xPos, yPos);
    window.draw(m_batteryBackground);

    // Battery fill
    float fillWidth = 110.0f * batteryPercent;
    m_batteryFill.setSize(sf::Vector2f(fillWidth, 20));
    m_batteryFill.setPosition(xPos + 5, yPos + 5);

    // Color based on battery level
    if (batteryPercent > 0.5f) {
        m_batteryFill.setFillColor(sf::Color(0, 255, 100)); // Green
    }
    else if (batteryPercent > 0.25f) {
        m_batteryFill.setFillColor(sf::Color(255, 200, 0)); // Yellow
    }
    else {
        // Red and flicker effect
        int alpha = isFlickering ? 150 : 255;
        m_batteryFill.setFillColor(sf::Color(255, 0, 0, alpha));
    }

    window.draw(m_batteryFill);

    // Battery text
    std::stringstream ss;
    ss << "Battery: " << static_cast<int>(batteryPercent * 100) << "%";
    m_batteryText.setString(ss.str());

    sf::FloatRect textBounds = m_batteryText.getLocalBounds();
    m_batteryText.setPosition(
        xPos + 60 - textBounds.width / 2,
        yPos + 35
    );

    window.draw(m_batteryText);
}

void GameUI::drawCollectibleCounter(sf::RenderWindow& window, int collected, int total) {
    float xPos = 20.0f;
    float yPos = 65.0f;

    std::stringstream ss;
    ss << "Orbs: " << collected << "/" << total;

    sf::Text collectText;
    collectText.setFont(m_font);
    collectText.setString(ss.str());
    collectText.setCharacterSize(28);

    if (collected == total) {
        collectText.setFillColor(sf::Color(0, 255, 100));
    }
    else {
        collectText.setFillColor(sf::Color(255, 180, 0));
    }

    collectText.setOutlineColor(sf::Color::Black);
    collectText.setOutlineThickness(2.0f);
    collectText.setPosition(xPos, yPos);

    window.draw(collectText);

    float iconSize = 10.0f;
    float iconSpacing = 14.0f;
    float iconStartX = xPos;
    float iconY = yPos + 38.0f;

    int maxIcons = std::min(total, 15);

    for (int i = 0; i < maxIcons; ++i) {
        sf::CircleShape icon(iconSize / 2);
        icon.setPosition(iconStartX + i * iconSpacing, iconY);

        if (i < collected) {
            icon.setFillColor(sf::Color(255, 150, 0, 220));
            icon.setOutlineColor(sf::Color(255, 200, 100));
        }
        else {
            icon.setFillColor(sf::Color(60, 60, 60, 180));
            icon.setOutlineColor(sf::Color(30, 30, 30));
        }

        icon.setOutlineThickness(1.0f);
        window.draw(icon);
    }
}

void GameUI::drawWinScreen(sf::RenderWindow& window) {
    sf::Vector2u size = window.getSize();
    float centerX = size.x / 2.0f;
    float centerY = size.y / 2.0f;

    m_screenDimmer.setSize(sf::Vector2f(static_cast<float>(size.x), static_cast<float>(size.y)));
    window.draw(m_screenDimmer);

    sf::FloatRect textRect = m_winText.getLocalBounds();
    m_winText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    m_winText.setPosition(centerX, centerY - 150);
    window.draw(m_winText);

    m_restartButton.setPosition(centerX - 100, centerY - 25);
    window.draw(m_restartButton);

    sf::FloatRect rTextRect = m_restartText.getLocalBounds();
    m_restartText.setOrigin(rTextRect.left + rTextRect.width / 2.0f, rTextRect.top + rTextRect.height / 2.0f);
    m_restartText.setPosition(centerX, centerY);
    window.draw(m_restartText);

    m_exitButton.setPosition(centerX - 100, centerY + 50);
    window.draw(m_exitButton);

    sf::FloatRect eTextRect = m_exitText.getLocalBounds();
    m_exitText.setOrigin(eTextRect.left + eTextRect.width / 2.0f, eTextRect.top + eTextRect.height / 2.0f);
    m_exitText.setPosition(centerX, centerY + 75);
    window.draw(m_exitText);
}

int GameUI::checkButtonPress(int mouseX, int mouseY) {
    sf::Vector2f mousePos(static_cast<float>(mouseX), static_cast<float>(mouseY));

    if (m_restartButton.getGlobalBounds().contains(mousePos)) {
        return 1;
    }
    if (m_exitButton.getGlobalBounds().contains(mousePos)) {
        return 2;
    }
    return 0;
}