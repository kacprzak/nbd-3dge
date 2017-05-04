#include "Terrain.h"

#include <SDL.h>
#include <SDL_image.h>

Terrain::Terrain(int actorId, TransformationComponent* tr, RenderComponent* rd, const std::string& dataFolder)
    : GfxNode{actorId, tr, rd}
{
    std::vector<unsigned char> heights = getHeightData(dataFolder + rd->mesh, &m_x, &m_y);

    auto mesh = std::shared_ptr<Mesh>{Mesh::fromHeightmap(heights, m_x, m_y, m_amplitude, m_textureStrech)};
    setMesh(mesh);
}

std::vector<unsigned char> Terrain::getHeightData(const std::string& filename, int* w, int* h)
{
    SDL_Surface* surface = IMG_Load(filename.c_str());
    
    if (!surface) {
        throw std::runtime_error{"SDL_Image load error: " + std::string{IMG_GetError()}};
    }

    *w = surface->w;
    *h = surface->h;

    std::vector<unsigned char> heights;
    heights.resize(*w * *h);

    SDL_LockSurface(surface);
    Uint8 *pixels = (Uint8 *)surface->pixels;
    
    for (int y = 0; y < *h; ++y) {
        for (int x = 0; x < *w; ++x) {
            heights[y * *h + x] = pixels[(y * surface->w + x) * surface->format->BytesPerPixel];
        }
    }
    SDL_UnlockSurface(surface);
    SDL_FreeSurface(surface);

    return heights;
}
