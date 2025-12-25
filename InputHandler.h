#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

/**
 * @class InputHandler
 * @brief Manages user input (keyboard and mouse)
 * 
 * Single Responsibility: Handles input processing
 * Interface Segregation: Provides specific input queries
 */
class InputHandler {
public:
    InputHandler();

    /**
     * @brief Process SFML events
     */
    void processEvent(const sf::Event& event);

    /**
     * @brief Update input state (call once per frame)
     */
    void update();

    // Keyboard queries
    bool isKeyPressed(sf::Keyboard::Key key) const;
    bool isForwardPressed() const;
    bool isBackwardPressed() const;
    bool isLeftPressed() const;
    bool isRightPressed() const;

    // Mouse queries
    sf::Vector2i getMouseDelta() const { return m_mouseDelta; }
    bool isMouseCaptured() const { return m_mouseCaptured; }
    void setMouseCaptured(bool captured) { m_mouseCaptured = captured; }

    // Exit query
    bool shouldExit() const { return m_shouldExit; }
    void initializeMousePosition(int centerX, int centerY);

private:
    sf::Vector2i m_lastMousePos;
    sf::Vector2i m_mouseDelta;
    bool m_mouseCaptured;
    bool m_shouldExit;
};

#endif // INPUTHANDLER_H