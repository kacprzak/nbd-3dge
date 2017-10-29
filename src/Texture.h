#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>
#include <memory>
#include <string>

class Texture final
{
  public:
    Texture(const Texture&) = delete;
    Texture(Texture&& other);
    Texture& operator=(const Texture&) = delete;
    ~Texture();

    void bind(int textureUnit);
    void setClampToEdge();
    void setRepeat();

    int width() const { return m_w; }
    int height() const { return m_h; }

    static Texture create(const std::string& filename, const std::string& internalFormat,
                          bool clamp = false);

    static Texture create(const std::array<std::string, 6> filenames,
                          const std::string& internalFormat, bool clamp = false);

  private:
    Texture(GLenum target);

    static GLint internalFormatToInt(const std::string& internalFormat);

    GLenum m_target;
    GLuint m_textureId = 0;
    GLuint m_samplerId = 0;

    int m_w = -1;
    int m_h = -1;
};

#endif // TEXTURE_H
