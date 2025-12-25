#include "ScreenFlashEffect.h"
#include <cmath>
#include <algorithm>

ScreenFlashEffect::ScreenFlashEffect()
    : m_flashColor(sf::Color::Yellow),
    m_duration(0.5f),
    m_elapsed(0.0f),
    m_active(false) {

    // Flash overlay will be resized in draw() to match window size
    m_flashOverlay.setFillColor(sf::Color(255, 255, 255, 0));
}

void ScreenFlashEffect::trigger(const sf::Color& color, float duration) {
    m_flashColor = color;
    m_duration = duration;
    m_elapsed = 0.0f;
    m_active = true;
}

void ScreenFlashEffect::update(float deltaTime) {
    if (!m_active) return;

    m_elapsed += deltaTime;

    // Deactivate when duration is exceeded
    if (m_elapsed >= m_duration) {
        m_active = false;
        m_elapsed = 0.0f;
    }
}

void ScreenFlashEffect::draw(sf::RenderWindow& window) {
    if (!m_active) return;

    // Resize overlay to match window
    sf::Vector2u windowSize = window.getSize();
    m_flashOverlay.setSize(sf::Vector2f(
        static_cast<float>(windowSize.x),
        static_cast<float>(windowSize.y)
    ));
    m_flashOverlay.setPosition(0, 0);

    // Calculate alpha based on animation curve
    int alpha = calculateAlpha();

    // Apply color with calculated alpha
    sf::Color flashColor = m_flashColor;
    flashColor.a = static_cast<sf::Uint8>(alpha);
    m_flashOverlay.setFillColor(flashColor);

    // Draw the overlay
    window.draw(m_flashOverlay);
}

int ScreenFlashEffect::calculateAlpha() const {
    // Normalized time (0.0 to 1.0)
    float t = m_elapsed / m_duration;

    // Fast fade in (0.0 to 0.2) then slow fade out (0.2 to 1.0)
    float alpha;

    if (t < 0.2f) {
        // Quick fade in: 0 to 180 alpha in first 20% of duration
        alpha = (t / 0.2f) * 180.0f;
    }
    else {
        // Slow fade out: 180 to 0 alpha in remaining 80% of duration
        float fadeOutProgress = (t - 0.2f) / 0.8f;
        alpha = 180.0f * (1.0f - fadeOutProgress);
    }

    // Clamp to valid range
    if (alpha < 0.0f) alpha = 0.0f;
    if (alpha > 255.0f) alpha = 255.0f;

    return static_cast<int>(alpha);
}