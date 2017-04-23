#ifndef TERRAIN_H
#define TERRAIN_H

#include "Actor.h"

class Terrain : public Actor
{
 public:
    Terrain(const std::string& name, const std::string& heightMapPath);

 private:
    float getHeight(int x, int y) const;
    glm::vec3 getNormal(int x, int y) const;
    glm::vec2 getTexCoord(int x, int y) const;
    
    int m_x = 0;
    int m_y = 0;

    float m_amplitude = 120.0f;
    float m_textureStrech = 10.0;

    std::vector<unsigned char> m_heights;
};

#endif // TERRAIN_H
