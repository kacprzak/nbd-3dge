#include "Texture.h"

#include <iostream>
#include "Util.h"
#include <SFML/Graphics.hpp>

Texture::Texture(GLenum target)
    : m_target(target)
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

void Texture::bind(GLenum textureUnit)
{
    glActiveTexture(textureUnit);
    glBindTexture(m_target, m_textureId);
    glBindSampler(0, m_samplerId);
}

void Texture::setClampToEdge()
{
    glSamplerParameteri(m_samplerId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(m_samplerId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(m_samplerId, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void Texture::setRepeat()
{
    glSamplerParameteri(m_samplerId, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glSamplerParameteri(m_samplerId, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glSamplerParameteri(m_samplerId, GL_TEXTURE_WRAP_R, GL_REPEAT);
}

Texture *Texture::create(const std::string &filename, bool clamp)
{
    GLenum target = GL_TEXTURE_2D;
    Texture *tex = new Texture{target};
    tex->m_filename = extractFilename(filename);

    sf::Image img;
    img.loadFromFile(filename);
    img.flipVertically();
    int w = img.getSize().x;
    int h = img.getSize().y;

    glBindTexture(target, tex->m_textureId);

    glTexImage2D(target, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.getPixelsPtr());
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

Texture *Texture::create(const std::array<std::string, 6> filenames, bool clamp)
{
    GLenum target = GL_TEXTURE_CUBE_MAP;
    Texture *tex = new Texture{target};
    tex->m_filename = extractFilename(filenames[0]);

    glBindTexture(target, tex->m_textureId);

    for (size_t i = 0; i < filenames.size(); ++i) {
        sf::Image img;
        img.loadFromFile(filenames[i]);
        //img.flipVertically();
        int w = img.getSize().x;
        int h = img.getSize().y;

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                     0, GL_RGB8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.getPixelsPtr());
    
        std::cout << "Loaded: " << extractFilename(filenames[i]) << " texId: "<< tex->m_textureId
                  << " (" << w << " x " << h << ")" << std::endl;
    }

    glSamplerParameteri(tex->m_samplerId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(tex->m_samplerId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (clamp)
        tex->setClampToEdge();
    else
        tex->setRepeat();

    return tex;
}

