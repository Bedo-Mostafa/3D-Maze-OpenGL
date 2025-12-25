#include "AudioManager.h"
#include <algorithm>
#include <iostream>

namespace
{
	inline float clampFloat(float value, float min, float max)
	{
		if (value < min) return min;
		if (value > max) return max;
		return value;
	}
}

AudioManager::AudioManager()
	: m_masterVolume(100.0f),
	m_musicMasterVolume(100.0f),
	m_soundMasterVolume(100.0f)
{
}

AudioManager::~AudioManager()
{
	stopMusic();
	m_activeSounds.clear();
}

bool AudioManager::initialize()
{
	// Force SFML to initialize OpenAL device/context
	static sf::SoundBuffer dummyBuffer;
	static sf::Sound dummySound;
	dummySound.setBuffer(dummyBuffer);
	m_isInitialized = true;   // ADD THIS

	std::cout << "Audio Manager initialized" << std::endl;
	return true;
}

bool AudioManager::loadSound(const std::string& name, const std::string& filepath)
{
	sf::SoundBuffer buffer;
	if (!buffer.loadFromFile(filepath))
	{
		std::cerr << "WARNING: Failed to load sound: " << filepath << std::endl;
		return false;
	}

	m_soundBuffers[name] = std::move(buffer);
	std::cout << "Loaded sound: " << name << std::endl;
	return true;
}

bool AudioManager::loadMusic(const std::string& name, const std::string& filepath)
{
	auto music = std::make_unique<sf::Music>();
	if (!music->openFromFile(filepath))
	{
		std::cerr << "WARNING: Failed to load music: " << filepath << std::endl;
		return false;
	}

	m_musicTracks[name] = std::move(music);
	std::cout << "Loaded music: " << name << std::endl;
	return true;
}

void AudioManager::playSound(const std::string& name, float volume)
{
	auto it = m_soundBuffers.find(name);
	if (it == m_soundBuffers.end())
	{
		std::cerr << "WARNING: Sound not found: " << name << std::endl;
		return;
	}

	auto sound = std::make_unique<sf::Sound>();
	sound->setBuffer(it->second);

	float finalVolume =
		volume *
		clampFloat(m_soundMasterVolume, 0.0f, 100.0f) / 100.0f;

	sound->setVolume(finalVolume);
	sound->play();

	m_activeSounds.push_back(std::move(sound));
}

void AudioManager::playSoundAtPosition(const std::string& name,
	const Vector3& position,
	const Vector3& listenerPos,
	float maxDistance)
{
	auto it = m_soundBuffers.find(name);
	if (it == m_soundBuffers.end())
		return;

	float volume = calculate3DVolume(position, listenerPos, maxDistance, 100.0f);

	if (volume > 1.0f)
		playSound(name, volume);
}

void AudioManager::playMusic(const std::string& name, bool loop, float volume)
{
	auto it = m_musicTracks.find(name);
	if (it == m_musicTracks.end())
	{
		std::cerr << "WARNING: Music not found: " << name << std::endl;
		return;
	}

	stopMusic();

	float finalVolume =
		volume *
		clampFloat(m_musicMasterVolume, 0.0f, 100.0f) / 100.0f *
		clampFloat(m_masterVolume, 0.0f, 100.0f) / 100.0f;

	it->second->setLoop(loop);
	it->second->setVolume(finalVolume);
	it->second->play();
}

void AudioManager::stopMusic()
{
	for (auto& pair : m_musicTracks)
	{
		if (pair.second->getStatus() == sf::Music::Playing)
			pair.second->stop();
	}
}

void AudioManager::setMusicVolume(float volume)
{
	float clamped = clampFloat(volume, 0.0f, 100.0f);

	for (auto& pair : m_musicTracks)
	{
		pair.second->setVolume(
			clamped *
			m_musicMasterVolume / 100.0f *
			m_masterVolume / 100.0f
		);
	}
}

void AudioManager::updateListenerPosition(const Vector3& position,
	const Vector3& forward)
{
	if (!m_isInitialized) return; // SAFETY GUARD
	// Prevent invalid OpenAL state
	if (forward.length() == 0.0f)
		return;

	sf::Listener::setPosition(position.x, position.y, position.z);
	sf::Listener::setDirection(forward.x, forward.y, forward.z);
}

void AudioManager::update()
{
	m_activeSounds.erase(
		std::remove_if(m_activeSounds.begin(), m_activeSounds.end(),
			[](const std::unique_ptr<sf::Sound>& sound)
			{
				return sound->getStatus() == sf::Sound::Stopped;
			}),
		m_activeSounds.end()
	);
}

void AudioManager::setMasterVolume(float volume)
{
	m_masterVolume = clampFloat(volume, 0.0f, 100.0f);
	setMusicVolume(50.0f);
}

void AudioManager::setMusicMasterVolume(float volume)
{
	m_musicMasterVolume = clampFloat(volume, 0.0f, 100.0f);
}

void AudioManager::setSoundMasterVolume(float volume)
{
	m_soundMasterVolume = clampFloat(volume, 0.0f, 100.0f);
}

float AudioManager::calculate3DVolume(const Vector3& soundPos,
	const Vector3& listenerPos,
	float maxDistance,
	float baseVolume)
{
	float distance = (soundPos - listenerPos).length();

	if (distance >= maxDistance)
		return 0.0f;

	float attenuation = 1.0f - (distance / maxDistance);
	return baseVolume * attenuation;
}
