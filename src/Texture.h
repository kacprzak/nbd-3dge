#ifndef TEXTURE_H
#define TEXTURE_H

#include "TextureData.h"

#include <GL/glew.h>
#include <glm/fwd.hpp>

#include <memory>
#include <string>

class Texture final
{
    friend class Framebuffer;

  public:
    Texture(const TextureData& texData);
    Texture(const Texture&) = delete;
    Texture(Texture&& other);
    Texture& operator=(const Texture&) = delete;
    ~Texture();

    static Texture createShadowMap(glm::ivec2 size);

    void bind(int textureUnit);
    void setClampToEdge();
    void setRepeat();

    int width() const { return m_w; }
    int height() const { return m_h; }

  private:
    void create2D(const TextureData& texData);
    void createCube(const TextureData& texData);

    Texture(GLenum target);

    static GLint formatToInternalFormat(GLenum format, bool linearColor);

    static std::string formatToString(GLenum format);
    static std::string internalFormatToString(GLint internalFormat);

    GLenum m_target;
    GLuint m_textureId = 0;
    GLuint m_samplerId = 0;

    int m_w = -1;
    int m_h = -1;
};

#endif // TEXTURE_H
