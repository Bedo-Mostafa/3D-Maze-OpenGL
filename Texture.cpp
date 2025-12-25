#include "Texture.h"
#include <SFML/Graphics/Image.hpp>
#include <iostream>

Texture::Texture() : m_textureId(0) {}

Texture::~Texture() {
    if (m_textureId != 0) {
        glDeleteTextures(1, &m_textureId);
    }
}

Texture::Texture(Texture&& other) noexcept : m_textureId(other.m_textureId) {
    other.m_textureId = 0;
}

Texture& Texture::operator=(Texture&& other) noexcept {
    if (this != &other) {
        if (m_textureId != 0) {
            glDeleteTextures(1, &m_textureId);
        }
        m_textureId = other.m_textureId;
        other.m_textureId = 0;
    }
    return *this;
}

bool Texture::loadFromFile(const std::string& filepath, bool generateMipmap) {
    // Load image using SFML
    sf::Image image;
    if (!image.loadFromFile(filepath)) {
        std::cerr << "Failed to load texture: " << filepath << std::endl;
        return false;
    }

    // Flip image vertically (OpenGL expects origin at bottom-left)
    image.flipVertically();

    // Get image data
    sf::Vector2u size = image.getSize();
    const sf::Uint8* pixels = image.getPixelsPtr();

    // Generate OpenGL texture
    glGenTextures(1, &m_textureId);
    glBindTexture(GL_TEXTURE_2D, m_textureId);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, generateMipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Upload texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    // Generate mipmaps if requested
    if (generateMipmap) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    std::cout << "Loaded texture: " << filepath << " (" << size.x << "x" << size.y << ")" << std::endl;
    return true;
}

void Texture::bind(unsigned int unit) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, m_textureId);
}

void Texture::unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}