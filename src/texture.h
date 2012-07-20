#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>
#include <string>

class Texture
{
public:
    ~Texture();

    void bind();

    static Texture *create(const std::string& filename);

private:
    Texture();

    std::string m_filename;
    GLuint m_textureId;
};

#endif // TEXTURE_H
