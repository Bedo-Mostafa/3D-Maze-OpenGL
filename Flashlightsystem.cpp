#include "FlashlightSystem.h"
#include <cmath>
#include <algorithm>

FlashlightSystem::FlashlightSystem()
    : m_isOn(true),
    m_battery(1.0f),
    m_maxBattery(1.0f),
    m_drainRate(0.05f),  // 20 seconds of battery life (1.0 / 0.05)
    m_flickerTimer(0.0f) {
}

void FlashlightSystem::update(float deltaTime) {
    if (!m_isOn) return;

    // Drain battery
    m_battery -= m_drainRate * deltaTime;
    m_battery = std::max(0.0f, m_battery);

    // Update flicker timer
    m_flickerTimer += deltaTime;

    // Auto turn off when dead
    if (m_battery <= 0.0f) {
        m_isOn = false;
    }
}

void FlashlightSystem::recharge(float amount) {
    m_battery += amount;
    m_battery = std::min(m_battery, m_maxBattery);
}

void FlashlightSystem::toggle() {
    if (m_battery > 0.0f) {
        m_isOn = !m_isOn;
    }
}

void FlashlightSystem::turnOn() {
    if (m_battery > 0.0f) {
        m_isOn = true;
    }
}

void FlashlightSystem::turnOff() {
    m_isOn = false;
}

float FlashlightSystem::getIntensity(float time) const {
    if (!m_isOn || m_battery <= 0.0f) {
        return 0.0f;
    }

    // Full intensity when battery is good
    if (m_battery > 0.25f) {
        return 1.5f; // Normal flashlight intensity
    }

    // Flicker when low battery
    float baseIntensity = 0.8f + (m_battery / 0.25f) * 0.7f; // 0.8 to 1.5

    // Add flicker effect
    float flicker = std::sin(time * 15.0f) * 0.3f +
        std::sin(time * 7.0f) * 0.2f;

    return baseIntensity + flicker * (1.0f - m_battery / 0.25f);
}