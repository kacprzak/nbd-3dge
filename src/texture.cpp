#include "texture.h"
#include <iostream>
#include "util.h"
#include <SFML/Graphics.hpp>

Texture::Texture()
  : m_textureId(0)
{
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_textureId);
    std::cout << "Released: " << m_filename << " texId: "<< m_textureId << std::endl;
}

void Texture::bind()
{
    glBindTexture(GL_TEXTURE_2D, m_textureId);
}

Texture *Texture::create(const std::string &fileName)
{
    Texture *tex = new Texture;
    tex->m_filename = extractFilename(fileName);

    sf::Image img;
    img.loadFromFile(fileName);
    img.flipVertically();
    int w = img.getSize().x;
    int h = img.getSize().y;

    glGenTextures(1, &tex->m_textureId);
    glBindTexture(GL_TEXTURE_2D, tex->m_textureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.getPixelsPtr());

    std::cout << "Loaded: " << tex->m_filename << " texId: "<< tex->m_textureId
              << " (" << w << " x " << h << ")" << std::endl;

    return tex;
}
