#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <gl/glew/glew.h>

/**
 * @class Texture
 * @brief Manages OpenGL textures (loading, binding, and resource management)
 * 
 * Single Responsibility: Handles texture operations
 */
class Texture {
public:
    Texture();
    ~Texture();

    // Disable copy operations
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    // Enable move operations
    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;

    /**
     * @brief Load texture from image file
     * @param filepath Path to image file
     * @param generateMipmap Whether to generate mipmaps
     * @return true if successful, false otherwise
     */
    bool loadFromFile(const std::string& filepath, bool generateMipmap = true);

    /**
     * @brief Bind this texture to the specified texture unit
     * @param unit Texture unit (0-31)
     */
    void bind(unsigned int unit = 0) const;

    /**
     * @brief Unbind texture from the current texture unit
     */
    void unbind() const;

    /**
     * @brief Get the OpenGL texture ID
     */
    GLuint getTextureId() const { return m_textureId; }

    /**
     * @brief Check if texture is valid
     */
    bool isValid() const { return m_textureId != 0; }

private:
    GLuint m_textureId;
};

#endif // TEXTURE_H