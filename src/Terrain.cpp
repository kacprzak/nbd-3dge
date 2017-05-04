#include "Terrain.h"

#include <SDL.h>
#include <SDL_image.h>

Terrain::Terrain(int actorId, TransformationComponent* tr, RenderComponent* rd, const std::string& dataFolder)
    : GfxNode{actorId, tr, rd}
{
    SDL_Surface* surface = IMG_Load((dataFolder + rd->mesh).c_str());
    
    if (!surface) {
        throw std::runtime_error("SDL_Image load error: " +
                                 std::string(IMG_GetError()));
    }

    m_x = surface->w;
    m_y = surface->h;

    std::vector<unsigned char> heights;
    heights.resize(m_x * m_y);

    SDL_LockSurface(surface);
    Uint8 *pixels = (Uint8 *)surface->pixels;
    
    for (int y = 0; y < m_y; ++y) {
        for (int x = 0; x < m_x; ++x) {
            heights[y * m_y + x] = pixels[(y * surface->w + x) * surface->format->BytesPerPixel];
        }
    }
    SDL_UnlockSurface(surface);

    SDL_FreeSurface(surface);

    auto mesh = std::shared_ptr<Mesh>{Mesh::fromHeightmap(heights, m_x, m_y, m_amplitude, m_textureStrech)};
    setMesh(mesh);
}
