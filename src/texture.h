#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>
#include <string>
#include <memory>

class Texture
{
public:
    ~Texture();

    void bind();
    void setClampToEdge();
    void setRepeat();

    static Texture *create(const std::string& filename);

private:
    Texture();

    std::string m_filename;
    GLuint m_textureId;
    GLuint m_samplerId;
};

typedef std::shared_ptr<Texture> TexturePtr;

#endif // TEXTURE_H
