#include "Terrain.h"

#include <SDL.h>
#include <SDL_image.h>

Terrain::Terrain(int actorId, TransformationComponent* tr, RenderComponent* rd,
                 const std::string& dataFolder)
    : GfxNode{actorId, tr, rd}
{
    const auto& heights = getHeightData(dataFolder + rd->mesh, &m_x, &m_y, m_amplitude);

    auto mesh = std::shared_ptr<Mesh>{Mesh::fromHeightmap(heights, m_x, m_y, m_textureStrech)};
    setMesh(mesh);
}

std::vector<float> Terrain::getHeightData(const std::string& filename, int* w, int* h, float amp)
{
    SDL_Surface* surface = IMG_Load(filename.c_str());

    if (!surface) {
        throw std::runtime_error{"SDL_Image load error: " + std::string{IMG_GetError()}};
    }

    *w = surface->w;
    *h = surface->h;

    std::vector<float> heights;
    heights.resize(*w * *h);

    SDL_LockSurface(surface);
    Uint8* pixels = (Uint8*)surface->pixels;

    for (int y = 0; y < *h; ++y) {
        for (int x = 0; x < *w; ++x) {
            auto& val = heights[y * *h + x];
            val       = pixels[(y * surface->w + x) * surface->format->BytesPerPixel] / 128.0 - 1.0;
            val *= amp;
        }
    }
    SDL_UnlockSurface(surface);
    SDL_FreeSurface(surface);

    return heights;
}
