#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>
#include <string>
#include <memory>

class Texture
{
public:
    ~Texture();

    void bind(GLenum textureUnit);
    void setClampToEdge();
    void setRepeat();

    static Texture *create(const std::string& filename, bool clamp = false);
    static Texture *create(const std::array<std::string, 6> filenames, bool clamp = false);
    
private:
    Texture(GLenum target);

    std::string m_filename;
    GLenum m_target;
    GLuint m_textureId;
    GLuint m_samplerId;
};

#endif // TEXTURE_H
