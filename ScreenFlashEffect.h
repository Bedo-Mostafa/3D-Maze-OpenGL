#pragma once
#include <SFML/Graphics.hpp>

class ScreenFlashEffect {
public:
    ScreenFlashEffect();

    // Trigger a flash with specified color and duration
    void trigger(const sf::Color& color, float duration = 0.5f);

    // Update the flash animation
    void update(float deltaTime);

    // Draw the flash overlay
    void draw(sf::RenderWindow& window);

    // Check if flash is currently active
    bool isActive() const { return m_active; }

private:
    sf::RectangleShape m_flashOverlay;
    sf::Color m_flashColor;
    float m_duration;
    float m_elapsed;
    bool m_active;

    // Calculate alpha based on elapsed time (fade in/out curve)
    int calculateAlpha() const;
};