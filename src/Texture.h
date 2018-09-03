#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>
#include <glm/fwd.hpp>

#include <memory>
#include <string>

class Sampler final
{
  public:
    Sampler();
    Sampler(const Sampler&) = delete;
    Sampler(Sampler&& other);
    Sampler& operator=(const Sampler&) = delete;
    ~Sampler();

    void bind(int textureUnit);

    void setClampToEdge();
    void setClampToBorder();
    void setRepeat();

    void setBorderColor(glm::vec4 color);

    void setParameter(GLenum param, GLint value);

  private:
    GLuint m_samplerId = 0;
};

//------------------------------------------------------------------------------

class Texture final
{
    friend class Framebuffer;

  public:
    Texture(const char* filename, const std::string& name = "");
    Texture(const Texture&) = delete;
    Texture(Texture&& other);
    Texture& operator=(const Texture&) = delete;
    ~Texture();

    static Texture createShadowMap(glm::ivec2 size);
    static Texture createShadowMap(glm::ivec3 size);

    void bind(int textureUnit);

    int width() const { return m_w; }
    int height() const { return m_h; }

    void setSampler(std::shared_ptr<Sampler> sampler) { m_sampler = sampler; }
    std::shared_ptr<Sampler> sampler()
    {
        if (!m_sampler) createSampler();
        return m_sampler;
    }

    std::string name;

  private:
    void createTexture(const char* filename);
    void createSampler();

    Texture(GLenum target);

    GLenum m_target;
    GLuint m_textureId = 0;

    int m_w      = -1;
    int m_h      = -1;
    int m_levels = 1;

    std::shared_ptr<Sampler> m_sampler;
};

#endif // TEXTURE_H
