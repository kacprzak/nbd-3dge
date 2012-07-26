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
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_textureId);
    glBindSampler(0, m_samplerId);
}

void Texture::setClampToEdge()
{
    //glSamplerParameteri(m_samplerId, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //glSamplerParameteri(m_samplerId, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glSamplerParameteri(m_samplerId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(m_samplerId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(m_samplerId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(m_samplerId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void Texture::setRepeat()
{
    glSamplerParameteri(m_samplerId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(m_samplerId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(m_samplerId, GL_TEXTURE_WRAP_S,     GL_REPEAT);
    glSamplerParameteri(m_samplerId, GL_TEXTURE_WRAP_T,     GL_REPEAT);
}

Texture *Texture::create(const std::string &fileName, bool clamp)
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

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.getPixelsPtr());

    std::cout << "Loaded: " << tex->m_filename << " texId: "<< tex->m_textureId
              << " (" << w << " x " << h << ")" << std::endl;

    glGenSamplers(1, &tex->m_samplerId);

    if (clamp)
        tex->setClampToEdge();
    else
        tex->setRepeat();

    return tex;
}
