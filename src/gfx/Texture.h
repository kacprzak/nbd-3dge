#ifndef TEXTURE_H
#define TEXTURE_H

#include "Macros.h"

#include <GL/glew.h>
#include <glm/fwd.hpp>

#include <array>
#include <filesystem>
#include <memory>
#include <string>

namespace gfx {

class Sampler final
{
    OSTREAM_FRIEND(Sampler);

  public:
    Sampler();
    Sampler(const Sampler&) = delete;
    Sampler(Sampler&& other);
    Sampler& operator=(const Sampler&) = delete;
    Sampler& operator=(Sampler&&) = delete;
    ~Sampler();

    void bind(int textureUnit);

    void setClampToEdge();
    void setClampToBorder();
    void setRepeat();

    void setBorderColor(glm::vec4 color);

    void setParameter(GLenum param, GLint value);

    static std::shared_ptr<Sampler> getDefault(bool mipmaps);

  private:
    GLuint m_samplerId = 0;
};

OSTREAM_IMPL_1(gfx::Sampler, m_samplerId)

//------------------------------------------------------------------------------

class Texture final
{
    OSTREAM_FRIEND(Texture);
    friend class Framebuffer;

  public:
    Texture(const std::filesystem::path& file, const std::string& name = "");
    // Creates one pixel texture
    Texture(glm::vec3 color);
    Texture(const Texture&) = delete;
    Texture(Texture&& other);
    Texture& operator=(const Texture&) = delete;
    Texture& operator=(Texture&&) = delete;
    ~Texture();

    static Texture createShadowMap(glm::ivec2 size);
    static Texture createShadowMap(glm::ivec3 size);

    void bind(int textureUnit);

    int width() const { return m_w; }
    int height() const { return m_h; }

    void setSampler(std::shared_ptr<Sampler> sampler) { m_sampler = sampler; }
    std::shared_ptr<Sampler> sampler() { return m_sampler; }

    std::string name;

  private:
    void createTexture(const char* filename);

    Texture(GLenum target, const std::string& name = "");

    GLenum m_target;
    GLuint m_textureId = 0;

    int m_w      = -1;
    int m_h      = -1;
    int m_levels = 1;

    std::shared_ptr<Sampler> m_sampler;
};

OSTREAM_IMPL_2(gfx::Texture, m_textureId, name)

//------------------------------------------------------------------------------

enum TextureUnit {
    BaseColor,
    Normal,
    Occlusion,
    Emissive,
    MetallicRoughness,
    Radiance,
    Irradiance,
    BrdfLUT = 7,
    Size
};

//------------------------------------------------------------------------------

using TexturePack = std::array<std::shared_ptr<Texture>, TextureUnit::Size>;

} // namespace gfx

#endif // TEXTURE_H
