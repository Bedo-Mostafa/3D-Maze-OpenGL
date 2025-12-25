#ifndef FLASHLIGHTSYSTEM_H
#define FLASHLIGHTSYSTEM_H

class FlashlightSystem {
public:
    FlashlightSystem();

    void update(float deltaTime);
    void recharge(float amount); // Pickup battery packs

    // State queries
    bool isOn() const { return m_isOn && m_battery > 0.0f; }
    float getBatteryPercent() const { return m_battery; }
    bool isLowBattery() const { return m_battery < 0.25f && m_battery > 0.0f; }
    bool isDead() const { return m_battery <= 0.0f; }

    // Toggle flashlight
    void toggle();
    void turnOn();
    void turnOff();

    // Get flashlight intensity (flickers when low)
    float getIntensity(float time) const;

    // Settings
    void setDrainRate(float rate) { m_drainRate = rate; }
    void setMaxBattery(float max) { m_maxBattery = max; }

private:
    bool m_isOn;
    float m_battery;        // 0.0 to 1.0 (or maxBattery)
    float m_maxBattery;     // Maximum battery
    float m_drainRate;      // Battery drain per second when on
    float m_flickerTimer;   // For low battery flicker effect
};

#endif // FLASHLIGHTSYSTEM_H