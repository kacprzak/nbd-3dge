#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>
#include <string>
#include <memory>

class Texture final
{
public:
    ~Texture();

    void bind(int textureUnit);
    void setClampToEdge();
    void setRepeat();

    int width() const { return m_w; }
    int height() const { return m_h; }
    
    static Texture *create(const std::string& filename, bool clamp = false);
    static Texture *create(const std::array<std::string, 6> filenames, bool clamp = false);
    
private:
    Texture(GLenum target);

    std::string m_filename;
    GLenum m_target;
    GLuint m_textureId;
    GLuint m_samplerId;

    int m_w = -1;
    int m_h = -1;
};

#endif // TEXTURE_H
