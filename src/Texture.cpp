#include "Texture.h"

#include <iostream>
#include "Util.h"
#include <SFML/Graphics.hpp>

Texture::Texture()
  : m_textureId(0)
{
    glGenTextures(1, &m_textureId);
    glGenSamplers(1, &m_samplerId);
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_textureId);
    glDeleteSamplers(1, &m_samplerId);
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
    glSamplerParameteri(m_samplerId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(m_samplerId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void Texture::setRepeat()
{
    glSamplerParameteri(m_samplerId, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glSamplerParameteri(m_samplerId, GL_TEXTURE_WRAP_T, GL_REPEAT);
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

    glBindTexture(GL_TEXTURE_2D, tex->m_textureId);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.getPixelsPtr());
    glGenerateMipmap(GL_TEXTURE_2D);
    
    std::cout << "Loaded: " << tex->m_filename << " texId: "<< tex->m_textureId
              << " (" << w << " x " << h << ")" << std::endl;

    glSamplerParameteri(tex->m_samplerId, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glSamplerParameteri(tex->m_samplerId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (clamp)
        tex->setClampToEdge();
    else
        tex->setRepeat();

    return tex;
}
