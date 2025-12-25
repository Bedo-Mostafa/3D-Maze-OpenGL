#include "InputHandler.h"

InputHandler::InputHandler()
	: m_lastMousePos(0, 0),
	m_mouseDelta(0, 0),
	m_mouseCaptured(true),
	m_shouldExit(false) {
}

void InputHandler::processEvent(const sf::Event& event) {
	if (event.type == sf::Event::Closed) {
		m_shouldExit = true;
	}
	else if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::Escape) {
			m_shouldExit = true;
		}
	}
	else if (event.type == sf::Event::MouseMoved) {
		if (m_mouseCaptured) {
			sf::Vector2i currentPos(event.mouseMove.x, event.mouseMove.y);
			m_mouseDelta = currentPos - m_lastMousePos;
			m_lastMousePos = currentPos;
		}
	}
}

void InputHandler::update() {
	m_mouseDelta = sf::Vector2i(0, 0);
}

bool InputHandler::isKeyPressed(sf::Keyboard::Key key) const {
	return sf::Keyboard::isKeyPressed(key);
}

bool InputHandler::isForwardPressed() const {
	return isKeyPressed(sf::Keyboard::W) || isKeyPressed(sf::Keyboard::Up);
}

bool InputHandler::isBackwardPressed() const {
	return isKeyPressed(sf::Keyboard::S) || isKeyPressed(sf::Keyboard::Down);
}

bool InputHandler::isLeftPressed() const {
	return isKeyPressed(sf::Keyboard::A) || isKeyPressed(sf::Keyboard::Left);
}

bool InputHandler::isRightPressed() const {
	return isKeyPressed(sf::Keyboard::D) || isKeyPressed(sf::Keyboard::Right);
}

void InputHandler::initializeMousePosition(int centerX, int centerY) {
	m_lastMousePos = sf::Vector2i(centerX, centerY);
	m_mouseDelta = sf::Vector2i(0, 0);
}