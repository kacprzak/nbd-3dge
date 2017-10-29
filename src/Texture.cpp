#include "Texture.h"

#include "Logger.h"

#include <SDL.h>
#include <SDL_image.h>

#include <array>

static GLenum textureFormat(SDL_Surface** surface);
static int SDL_InvertSurface(SDL_Surface* image);

//==============================================================================

Texture::Texture(GLenum target)
    : m_target(target)
{
    glGenTextures(1, &m_textureId);
    glGenSamplers(1, &m_samplerId);
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

Texture Texture::create(const std::string& filename, const std::string& internalFormat, bool clamp)
{
    GLenum target = GL_TEXTURE_2D;
    Texture tex{target};

    SDL_Surface* surface = IMG_Load(filename.c_str());

    if (!surface) {
        throw std::runtime_error("SDL_Image load error: " + std::string(IMG_GetError()));
    }

    // Top down inversion
    if (SDL_InvertSurface(surface) != 0) {
        SDL_FreeSurface(surface);
        throw std::runtime_error("SDL Error: " + std::string(SDL_GetError()));
    }

    tex.m_w = surface->w;
    tex.m_h = surface->h;

    glBindTexture(target, tex.m_textureId);

    GLenum format   = textureFormat(&surface);
    GLint intFormat = internalFormatToInt(internalFormat);

    SDL_LockSurface(surface);
    glTexImage2D(target, 0, intFormat, tex.m_w, tex.m_h, 0, format, GL_UNSIGNED_BYTE,
                 surface->pixels);
    SDL_UnlockSurface(surface);

    SDL_FreeSurface(surface);

    glGenerateMipmap(GL_TEXTURE_2D);

    LOG_INFO << "Loaded Texture: " << tex.m_textureId << " (" << tex.m_w << " x " << tex.m_h << ")";

    glSamplerParameteri(tex.m_samplerId, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glSamplerParameteri(tex.m_samplerId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (clamp)
        tex.setClampToEdge();
    else
        tex.setRepeat();

    return tex;
}

Texture Texture::create(const std::array<std::string, 6> filenames,
                        const std::string& internalFormat, bool clamp)
{
    GLenum target = GL_TEXTURE_CUBE_MAP;
    Texture tex{target};

    glBindTexture(target, tex.m_textureId);

    for (size_t i = 0; i < filenames.size(); ++i) {
        SDL_Surface* surface = IMG_Load(filenames[i].c_str());

        if (!surface) {
            throw std::runtime_error("SDL_Image load error: " + std::string(IMG_GetError()));
        }

        tex.m_w = surface->w;
        tex.m_h = surface->h;

        GLenum format   = textureFormat(&surface);
        GLint intFormat = internalFormatToInt(internalFormat);

        SDL_LockSurface(surface);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, intFormat, tex.m_w, tex.m_h, 0, format,
                     GL_UNSIGNED_BYTE, surface->pixels);
        SDL_UnlockSurface(surface);

        SDL_FreeSurface(surface);
    }

    LOG_INFO << "Loaded CubeTex: " << tex.m_textureId << " (" << tex.m_w << " x " << tex.m_h << ")";

    glSamplerParameteri(tex.m_samplerId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(tex.m_samplerId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (clamp)
        tex.setClampToEdge();
    else
        tex.setRepeat();

    return tex;
}

//------------------------------------------------------------------------------

GLint Texture::internalFormatToInt(const std::string& internalFormat)
{
    if (internalFormat == "GL_RGB8")
        return GL_RGB8;
    else if (internalFormat == "GL_RGBA8")
        return GL_RGBA8;
    else if (internalFormat == "GL_SRGB8")
        return GL_SRGB8;
    else if (internalFormat == "GL_SRGB8_ALPHA8")
        return GL_SRGB8_ALPHA8;

    LOG_WARNING << "Unknown internalFormat: " << internalFormat;
    return GL_RGB8;
}

//==============================================================================

static GLenum textureFormat(SDL_Surface** surface)
{
    int format;

    // work out what format to tell glTexImage2D to use...
    if ((*surface)->format->BytesPerPixel == 3) { // RGB 24bit
        format                                      = GL_RGB;
        if ((*surface)->format->Bshift == 0) format = GL_BGR;
    } else if ((*surface)->format->BytesPerPixel == 4) { // RGBA 32bit
        format                                      = GL_RGBA;
        if ((*surface)->format->Bshift == 0) format = GL_BGRA;
    } else {
        // Convert to 32 bits.
        SDL_PixelFormat fmt;
        memset(&fmt, 0, sizeof(fmt));
        fmt.format        = SDL_PIXELFORMAT_RGBA8888;
        fmt.BitsPerPixel  = 32;
        fmt.BytesPerPixel = 4;
        fmt.Rmask         = 0xff;
        fmt.Gmask         = 0xff00;
        fmt.Bmask         = 0xff0000;
        fmt.Amask         = 0xff000000;

        SDL_Surface* nimg = SDL_ConvertSurface(*surface, &fmt, 0);

        if (!nimg) {
            LOG_WARNING << "SDL error: " << SDL_GetError();
            return 0;
        }

        // Done converting.
        SDL_FreeSurface(*surface);
        *surface = nimg;
        format   = GL_RGBA;
    }
    return format;
}

//------------------------------------------------------------------------------
// Code from
// http://www.gribblegames.com/articles/game_programming/sdlgl/invert_sdl_surfaces.html
//
static int invert_image(int pitch, int height, void* image_pixels)
{
    int index;
    void* temp_row;
    int height_div_2;

    temp_row = (void*)malloc(pitch);
    if (NULL == temp_row) {
        SDL_SetError("Not enough memory for image inversion");
        return -1;
    }
    // if height is odd, don't need to swap middle row
    height_div_2 = (int)(height * .5);
    for (index = 0; index < height_div_2; ++index) {
        // uses string.h
        memcpy((Uint8*)temp_row, (Uint8*)(image_pixels) + pitch * index, pitch);

        memcpy((Uint8*)(image_pixels) + pitch * index,
               (Uint8*)(image_pixels) + pitch * (height - index - 1), pitch);

        memcpy((Uint8*)(image_pixels) + pitch * (height - index - 1), temp_row, pitch);
    }
    free(temp_row);
    return 0;
}

//------------------------------------------------------------------------------

// This is the function you want to call!
static int SDL_InvertSurface(SDL_Surface* image)
{
    if (NULL == image) {
        SDL_SetError("Surface is NULL");
        return -1;
    }
    if (SDL_LockSurface(image) != 0) {
        return -1;
    }
    int err = invert_image(image->pitch, image->h, image->pixels);
    SDL_UnlockSurface(image);
    return err;
}
