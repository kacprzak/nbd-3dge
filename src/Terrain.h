#ifndef TERRAIN_H
#define TERRAIN_H

#include "GfxNode.h"

class Terrain : public GfxNode
{
 public:
    Terrain(int actorId, TransformationComponent* tr, RenderComponent* rd, const std::string& dataFolder);

 private:
    int m_x = 0;
    int m_y = 0;

    float m_amplitude = 80.0f;
    float m_textureStrech = 10.0;
};

#endif // TERRAIN_H
