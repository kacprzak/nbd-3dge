#include "Texture.h"

#include "Logger.h"

#include <SDL.h>

#include <gli/gl.hpp>
#include <gli/gli.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <array>

Sampler::Sampler()
{
    glGenSamplers(1, &m_samplerId);

    glSamplerParameteri(m_samplerId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(m_samplerId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    setRepeat();

    LOG_INFO("Created Sampler: {}", m_samplerId);
}

Sampler::Sampler(Sampler&& other) { std::swap(m_samplerId, other.m_samplerId); }

Sampler::~Sampler()
{
    glDeleteSamplers(1, &m_samplerId);

    if (m_samplerId) LOG_INFO("Released Sampler: {}", m_samplerId);
}

void Sampler::bind(int textureUnit) { glBindSampler(textureUnit, m_samplerId); }

void Sampler::setClampToEdge()
{
    glSamplerParameteri(m_samplerId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(m_samplerId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(m_samplerId, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void Sampler::setClampToBorder()
{
    glSamplerParameteri(m_samplerId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glSamplerParameteri(m_samplerId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glSamplerParameteri(m_samplerId, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
}

void Sampler::setRepeat()
{
    glSamplerParameteri(m_samplerId, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glSamplerParameteri(m_samplerId, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glSamplerParameteri(m_samplerId, GL_TEXTURE_WRAP_R, GL_REPEAT);
}

void Sampler::setBorderColor(glm::vec4 color)
{
    glSamplerParameterfv(m_samplerId, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(color));
}

void Sampler::setParameter(GLenum param, GLint value)
{
    glSamplerParameteri(m_samplerId, param, value);
}

//------------------------------------------------------------------------------

Texture::Texture(GLenum target)
    : m_target{target}
{
    glGenTextures(1, &m_textureId);
}

Texture::Texture(const std::filesystem::path& file, const std::string& _name)
    : Texture{GL_TEXTURE_2D}
{
    createTexture(file.string().c_str());

    name = _name.empty() ? file.filename().string() : _name;
    LOG_INFO("Created Texture: {} | {}", m_textureId, name);
}

Texture::Texture(Texture&& other)
{
    std::swap(m_target, other.m_target);
    std::swap(m_textureId, other.m_textureId);
    std::swap(m_w, other.m_w);
    std::swap(m_h, other.m_h);
    std::swap(m_sampler, other.m_sampler);
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_textureId);

    if (m_textureId) LOG_INFO("Released Texture: {}", m_textureId);
}

Texture Texture::createShadowMap(glm::ivec2 size)
{
    Texture tex{GL_TEXTURE_2D};
    LOG_INFO("Created Texture: {} | {}", tex.m_textureId, "ShadowMap2D");

    glBindTexture(tex.m_target, tex.m_textureId);
    glTexImage2D(tex.m_target, 0, GL_DEPTH_COMPONENT16, size.x, size.y, 0, GL_DEPTH_COMPONENT,
                 GL_FLOAT, NULL);

    auto sampler = std::make_shared<Sampler>();

    sampler->setClampToBorder();
    sampler->setBorderColor({1.0f, 1.0f, 1.0f, 1.0f});
    sampler->setParameter(GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);

    tex.setSampler(sampler);

    return tex;
}

Texture Texture::createShadowMap(glm::ivec3 size)
{
    Texture tex{GL_TEXTURE_2D_ARRAY};
    LOG_INFO("Created Texture: {} | {}", tex.m_textureId, "ShadowMap2DArray");

    glBindTexture(tex.m_target, tex.m_textureId);
    glTexImage3D(tex.m_target, 0, GL_DEPTH_COMPONENT16, size.x, size.y, size.z, 0,
                 GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    auto sampler = std::make_shared<Sampler>();

    sampler->setClampToBorder();
    sampler->setBorderColor({1.0f, 1.0f, 1.0f, 1.0f});
    sampler->setParameter(GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);

    tex.setSampler(sampler);

    return tex;
}

void Texture::bind(int textureUnit)
{
    if (!m_sampler) createSampler();

    m_sampler->bind(textureUnit);
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(m_target, m_textureId);
}

void Texture::createSampler()
{
    m_sampler = std::make_shared<Sampler>();
    if (m_levels > 1) {
        m_sampler->setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    }
}

// Filename can be KTX or DDS files
void Texture::createTexture(const char* filename)
{
    gli::texture tex = gli::load(filename);
    if (tex.empty()) throw std::runtime_error("Texture load error: " + std::string(filename));

    gli::gl GL(gli::gl::PROFILE_GL33);
    gli::gl::format Format = GL.translate(tex.format(), tex.swizzles());

    m_target = GL.translate(tex.target());

    glBindTexture(m_target, m_textureId);
    glTexParameteri(m_target, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(m_target, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(tex.levels() - 1));
    glTexParameteri(m_target, GL_TEXTURE_SWIZZLE_R, Format.Swizzles[0]);
    glTexParameteri(m_target, GL_TEXTURE_SWIZZLE_G, Format.Swizzles[1]);
    glTexParameteri(m_target, GL_TEXTURE_SWIZZLE_B, Format.Swizzles[2]);
    glTexParameteri(m_target, GL_TEXTURE_SWIZZLE_A, Format.Swizzles[3]);

    const glm::tvec3<GLsizei> extent(tex.extent());
    const GLsizei faceTotal = static_cast<GLsizei>(tex.layers() * tex.faces());

    m_w      = extent.x;
    m_h      = extent.y;
    m_levels = tex.levels();

    switch (tex.target()) {
    case gli::TARGET_1D:
        glTexStorage1D(m_target, static_cast<GLint>(tex.levels()), Format.Internal, extent.x);
        break;
    case gli::TARGET_1D_ARRAY:
    case gli::TARGET_2D:
    case gli::TARGET_CUBE:
        glTexStorage2D(m_target, static_cast<GLint>(tex.levels()), Format.Internal, extent.x,
                       tex.target() != gli::TARGET_1D_ARRAY ? extent.y : faceTotal);
        break;
    case gli::TARGET_2D_ARRAY:
    case gli::TARGET_3D:
    case gli::TARGET_CUBE_ARRAY:
        glTexStorage3D(m_target, static_cast<GLint>(tex.levels()), Format.Internal, extent.x,
                       extent.y, tex.target() == gli::TARGET_3D ? extent.z : faceTotal);
        break;
    default: assert(0); break;
    }

    for (std::size_t Layer = 0; Layer < tex.layers(); ++Layer) {
        for (std::size_t Face = 0; Face < tex.faces(); ++Face) {
            for (std::size_t Level = 0; Level < tex.levels(); ++Level) {
                GLsizei const LayerGL = static_cast<GLsizei>(Layer);
                glm::tvec3<GLsizei> extent(tex.extent(Level));
                auto target = gli::is_target_cube(tex.target())
                                  ? static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X + Face)
                                  : m_target;

                switch (tex.target()) {
                case gli::TARGET_1D:
                    if (gli::is_compressed(tex.format()))
                        glCompressedTexSubImage1D(
                            target, static_cast<GLint>(Level), 0, extent.x, Format.Internal,
                            static_cast<GLsizei>(tex.size(Level)), tex.data(Layer, Face, Level));
                    else
                        glTexSubImage1D(target, static_cast<GLint>(Level), 0, extent.x,
                                        Format.External, Format.Type, tex.data(Layer, Face, Level));
                    break;
                case gli::TARGET_1D_ARRAY:
                case gli::TARGET_2D:
                case gli::TARGET_CUBE:
                    if (gli::is_compressed(tex.format()))
                        glCompressedTexSubImage2D(
                            target, static_cast<GLint>(Level), 0, 0, extent.x,
                            tex.target() == gli::TARGET_1D_ARRAY ? LayerGL : extent.y,
                            Format.Internal, static_cast<GLsizei>(tex.size(Level)),
                            tex.data(Layer, Face, Level));
                    else
                        glTexSubImage2D(target, static_cast<GLint>(Level), 0, 0, extent.x,
                                        tex.target() == gli::TARGET_1D_ARRAY ? LayerGL : extent.y,
                                        Format.External, Format.Type, tex.data(Layer, Face, Level));
                    break;
                case gli::TARGET_2D_ARRAY:
                case gli::TARGET_3D:
                case gli::TARGET_CUBE_ARRAY:
                    if (gli::is_compressed(tex.format()))
                        glCompressedTexSubImage3D(
                            target, static_cast<GLint>(Level), 0, 0, 0, extent.x, extent.y,
                            tex.target() == gli::TARGET_3D ? extent.z : LayerGL, Format.Internal,
                            static_cast<GLsizei>(tex.size(Level)), tex.data(Layer, Face, Level));
                    else
                        glTexSubImage3D(target, static_cast<GLint>(Level), 0, 0, 0, extent.x,
                                        extent.y,
                                        tex.target() == gli::TARGET_3D ? extent.z : LayerGL,
                                        Format.External, Format.Type, tex.data(Layer, Face, Level));
                    break;
                default: assert(0); break;
                }
            }
        }
    }
}

