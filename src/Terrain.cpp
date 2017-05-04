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


    if (m_x % 2 != 0 || m_y % 2 != 0) {
        throw std::runtime_error{"Terrain heightMap with odd size is not supported."};
    }

    m_heights.resize(m_x * m_y);

    SDL_LockSurface(surface);
    Uint8 *pixels = (Uint8 *)surface->pixels;
    
    for (int y = 0; y < m_y; ++y) {
        for (int x = 0; x < m_x; ++x) {
            m_heights[y * m_y + x] = pixels[(y * surface->w + x) * surface->format->BytesPerPixel];
        }
    }
    SDL_UnlockSurface(surface);

    SDL_FreeSurface(surface);


    std::vector<GLfloat> vertices;
    vertices.resize(m_x * m_y * 3);

    for (int y = 0; y < m_y; ++y) {
        for (int x = 0; x < m_x; ++x) {
            int idx = (y * m_y + x) * 3;
            vertices[idx] = x - m_x/2.0f;
            vertices[idx + 1] = getHeight(x, y);
            vertices[idx + 2] = y - m_y/2.0f;
        }
    }

    std::vector<GLfloat> normals;
    normals.resize(m_x * m_y * 3);

    for (int y = 0; y < m_y; ++y) {
        for (int x = 0; x < m_x; ++x) {
            int idx = (y * m_y + x) * 3;
            auto n = getNormal(x, y);
            normals[idx] = n[0];
            normals[idx + 1] = n[1];
            normals[idx + 2] = n[2];
        }
    }

    std::vector<GLfloat> texCoords;
    texCoords.resize(m_x * m_y * 2);

    for (int y = 0; y < m_y; ++y) {
        for (int x = 0; x < m_x; ++x) {
            int idx = (y * m_y + x) * 2;
            auto t = getTexCoord(x, y);
            texCoords[idx] = t[0];
            texCoords[idx + 1] = t[1];
        }
    }
    
    std::vector<GLushort> indices;
    indices.resize(m_x * m_y + (m_x-1) * (m_y-2));

    int ind = 0;
    for (int y = 0; y < m_y - 1; ++y) {
        if (y % 2 == 0) {
            for (int x = 0; x < m_x; ++x) {
                indices[ind++] = x + y * m_y;
                indices[ind++] = x + (y+1) * m_y;
            }
        } else {
            for (int x = m_x - 1; x > 0; --x) {
                indices[ind++] = x + (y+1) * m_y;
                indices[ind++] = (x-1) + y * m_y;
            }
        }
    }

    auto mesh = std::make_shared<Mesh>(GL_TRIANGLE_STRIP,
                                       vertices, normals,
                                       texCoords, indices);

    setMesh(mesh);
}

float Terrain::getHeight(int x, int y) const
{
    return ((m_heights[y * m_y + x]/255.0f) - 0.5) * m_amplitude;
}

glm::vec3 Terrain::getNormal(int x, int y) const
{
    auto n =  glm::vec3{0.0f, 0.0f, 2.0};

    float center = getHeight(x, y);

    float left = x > 0 ? getHeight(x - 1, y) : center;
    float right = x < m_x - 1 ? getHeight(x + 1, y) : center;

    n[0] = right - left;

    float bottom = y > 0 ? getHeight(x, y - 1) : center;
    float top = y < m_y - 1 ? getHeight(x, y + 1) : center;

    n[1] = top - bottom;

    // W have normal in texture space where z is up but in OpenGL y is up
    n[0] = -n[0];
    n[2] = -n[1];
    n[1] = 2.0f;

    return glm::normalize(n);
}

glm::vec2 Terrain::getTexCoord(int x, int y) const
{
    auto texCoord = glm::vec2{0.0, 0.0};

    texCoord[0] = x / m_textureStrech;
    texCoord[1] = y / m_textureStrech;

    return texCoord;
}
