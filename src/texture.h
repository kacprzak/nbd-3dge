#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>
#include <string>
#include <SFML/Graphics.hpp>

class Texture
{
public:
    Texture();
    ~Texture();

    void bind();

    static Texture *create(const std::string& fileName);

private:
    std::string m_filename;
    GLuint m_textureId;
};

#endif // TEXTURE_H
