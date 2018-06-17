#include "Texture.h"

#include "Logger.h"

#include <SDL.h>

#include <glm/glm.hpp>
#include <gli/gl.hpp>
#include <gli/gli.hpp>

#include <array>

Texture::Texture(GLenum target)
    : m_target{target}
{
    glGenTextures(1, &m_textureId);
    glGenSamplers(1, &m_samplerId);

    glSamplerParameteri(m_samplerId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(m_samplerId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

Texture::Texture(const TextureData& texData)
    : Texture{GL_TEXTURE_2D}
{
    createTexture(texData.filename.c_str());

    if (m_levels > 1)
        glSamplerParameteri(m_samplerId, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    if (texData.clamp)
        setClampToEdge();
    else
        setRepeat();
}

Texture::Texture(Texture&& other)
{
    std::swap(m_target, other.m_target);
    std::swap(m_textureId, other.m_textureId);
    std::swap(m_samplerId, other.m_samplerId);
    std::swap(m_w, other.m_w);
    std::swap(m_h, other.m_h);
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_textureId);
    glDeleteSamplers(1, &m_samplerId);

    if (m_textureId || m_samplerId) LOG_INFO << "Released Texture: " << m_textureId;
}

Texture Texture::createShadowMap(glm::ivec2 size)
{
    Texture tex{GL_TEXTURE_2D};

    glBindTexture(tex.m_target, tex.m_textureId);
    glTexImage2D(tex.m_target, 0, GL_DEPTH_COMPONENT16, size.x, size.y, 0, GL_DEPTH_COMPONENT,
                 GL_FLOAT, NULL);

    glSamplerParameteri(tex.m_samplerId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(tex.m_samplerId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glSamplerParameteri(tex.m_samplerId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glSamplerParameteri(tex.m_samplerId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glSamplerParameterfv(tex.m_samplerId, GL_TEXTURE_BORDER_COLOR, borderColor);

    glSamplerParameteri(tex.m_samplerId, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);

    return tex;
}

Texture Texture::createShadowMap(glm::ivec3 size)
{
    Texture tex{GL_TEXTURE_2D_ARRAY};

    glBindTexture(tex.m_target, tex.m_textureId);
    glTexImage3D(tex.m_target, 0, GL_DEPTH_COMPONENT16, size.x, size.y, size.z, 0,
                 GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glSamplerParameteri(tex.m_samplerId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(tex.m_samplerId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glSamplerParameteri(tex.m_samplerId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glSamplerParameteri(tex.m_samplerId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glSamplerParameterfv(tex.m_samplerId, GL_TEXTURE_BORDER_COLOR, borderColor);

    glSamplerParameteri(tex.m_samplerId, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);

    return tex;
}

void Texture::bind(int textureUnit)
{
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(m_target, m_textureId);
    glBindSampler(textureUnit, m_samplerId);
}

void Texture::setClampToEdge()
{
    glSamplerParameteri(m_samplerId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(m_samplerId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(m_samplerId, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void Texture::setRepeat()
{
    glSamplerParameteri(m_samplerId, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glSamplerParameteri(m_samplerId, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glSamplerParameteri(m_samplerId, GL_TEXTURE_WRAP_R, GL_REPEAT);
}

//==============================================================================

// Filename can be KTX or DDS files
void Texture::createTexture(char const* Filename)
{
	gli::texture Texture = gli::load(Filename);
	if(Texture.empty())
            throw std::runtime_error("Texture load error: " + std::string(Filename));

	gli::gl GL(gli::gl::PROFILE_GL33);
	gli::gl::format const Format = GL.translate(Texture.format(), Texture.swizzles());
	m_target = GL.translate(Texture.target());

	//GLuint TextureName = 0;
	//glGenTextures(1, &TextureName);
	glBindTexture(m_target, m_textureId);
	glTexParameteri(m_target, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(m_target, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(Texture.levels() - 1));
	glTexParameteri(m_target, GL_TEXTURE_SWIZZLE_R, Format.Swizzles[0]);
	glTexParameteri(m_target, GL_TEXTURE_SWIZZLE_G, Format.Swizzles[1]);
	glTexParameteri(m_target, GL_TEXTURE_SWIZZLE_B, Format.Swizzles[2]);
	glTexParameteri(m_target, GL_TEXTURE_SWIZZLE_A, Format.Swizzles[3]);

	glm::tvec3<GLsizei> const Extent(Texture.extent());
	GLsizei const FaceTotal = static_cast<GLsizei>(Texture.layers() * Texture.faces());

    m_w = Extent.x;
    m_h = Extent.y;
    m_levels = Texture.levels();

	switch(Texture.target())
	{
	case gli::TARGET_1D:
		glTexStorage1D(
			m_target, static_cast<GLint>(Texture.levels()), Format.Internal, Extent.x);
		break;
	case gli::TARGET_1D_ARRAY:
	case gli::TARGET_2D:
	case gli::TARGET_CUBE:
		glTexStorage2D(
			m_target, static_cast<GLint>(Texture.levels()), Format.Internal,
            Extent.x, Texture.target() != gli::TARGET_1D_ARRAY ? Extent.y : FaceTotal);
		break;
	case gli::TARGET_2D_ARRAY:
	case gli::TARGET_3D:
	case gli::TARGET_CUBE_ARRAY:
		glTexStorage3D(
			m_target, static_cast<GLint>(Texture.levels()), Format.Internal,
			Extent.x, Extent.y,
			Texture.target() == gli::TARGET_3D ? Extent.z : FaceTotal);
		break;
	default:
		assert(0);
		break;
	}

	for(std::size_t Layer = 0; Layer < Texture.layers(); ++Layer)
	for(std::size_t Face = 0; Face < Texture.faces(); ++Face)
	for(std::size_t Level = 0; Level < Texture.levels(); ++Level)
	{
		GLsizei const LayerGL = static_cast<GLsizei>(Layer);
		glm::tvec3<GLsizei> Extent(Texture.extent(Level));
        auto target = gli::is_target_cube(Texture.target())
			? static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X + Face)
			: m_target;

		switch(Texture.target())
		{
		case gli::TARGET_1D:
			if(gli::is_compressed(Texture.format()))
				glCompressedTexSubImage1D(
                    target, static_cast<GLint>(Level), 0, Extent.x,
					Format.Internal, static_cast<GLsizei>(Texture.size(Level)),
					Texture.data(Layer, Face, Level));
			else
				glTexSubImage1D(
                    target, static_cast<GLint>(Level), 0, Extent.x,
					Format.External, Format.Type,
					Texture.data(Layer, Face, Level));
			break;
		case gli::TARGET_1D_ARRAY:
		case gli::TARGET_2D:
		case gli::TARGET_CUBE:
			if(gli::is_compressed(Texture.format()))
				glCompressedTexSubImage2D(
                    target, static_cast<GLint>(Level),
					0, 0,
					Extent.x,
					Texture.target() == gli::TARGET_1D_ARRAY ? LayerGL : Extent.y,
					Format.Internal, static_cast<GLsizei>(Texture.size(Level)),
					Texture.data(Layer, Face, Level));
			else
				glTexSubImage2D(
                    target, static_cast<GLint>(Level),
					0, 0,
					Extent.x,
					Texture.target() == gli::TARGET_1D_ARRAY ? LayerGL : Extent.y,
					Format.External, Format.Type,
					Texture.data(Layer, Face, Level));
			break;
		case gli::TARGET_2D_ARRAY:
		case gli::TARGET_3D:
		case gli::TARGET_CUBE_ARRAY:
			if(gli::is_compressed(Texture.format()))
				glCompressedTexSubImage3D(
                    target, static_cast<GLint>(Level),
					0, 0, 0,
					Extent.x, Extent.y,
					Texture.target() == gli::TARGET_3D ? Extent.z : LayerGL,
					Format.Internal, static_cast<GLsizei>(Texture.size(Level)),
					Texture.data(Layer, Face, Level));
			else
				glTexSubImage3D(
                    target, static_cast<GLint>(Level),
					0, 0, 0,
					Extent.x, Extent.y,
					Texture.target() == gli::TARGET_3D ? Extent.z : LayerGL,
					Format.External, Format.Type,
					Texture.data(Layer, Face, Level));
			break;
		default: assert(0); break;
		}
	}
}
