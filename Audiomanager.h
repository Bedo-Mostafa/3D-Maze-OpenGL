#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <SFML/Audio.hpp>
#include <memory>
#include <unordered_map>
#include <string>
#include "Vector3.h"

class AudioManager {
public:
	AudioManager();
	~AudioManager();

	bool initialize();
	bool m_isInitialized = false;


	// Load audio files
	bool loadSound(const std::string& name, const std::string& filepath);
	bool loadMusic(const std::string& name, const std::string& filepath);

	// Play sounds
	void playSound(const std::string& name, float volume = 100.0f);
	void playSoundAtPosition(const std::string& name, const Vector3& position,
		const Vector3& listenerPos, float maxDistance = 20.0f);

	// Music control
	void playMusic(const std::string& name, bool loop = true, float volume = 50.0f);
	void stopMusic();
	void setMusicVolume(float volume);

	// 3D audio (for portal hum, etc.)
	void updateListenerPosition(const Vector3& position, const Vector3& forward);

	// Update (for cleaning up finished sounds)
	void update();

	// Global volume control
	void setMasterVolume(float volume);
	void setMusicMasterVolume(float volume);
	void setSoundMasterVolume(float volume);

private:
	std::unordered_map<std::string, sf::SoundBuffer> m_soundBuffers;
	std::unordered_map<std::string, std::unique_ptr<sf::Music>> m_musicTracks;

	std::vector<std::unique_ptr<sf::Sound>> m_activeSounds;

	float m_masterVolume;
	float m_musicMasterVolume;
	float m_soundMasterVolume;

	// Calculate 3D attenuation
	float calculate3DVolume(const Vector3& soundPos, const Vector3& listenerPos,
		float maxDistance, float baseVolume);
};

#endif // AUDIOMANAGER_H