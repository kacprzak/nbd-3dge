#ifndef TERRAIN_H
#define TERRAIN_H

#include "GfxNode.h"
#include "Heightfield.h"

class Terrain : public GfxNode
{
  public:
    Terrain(int actorId, TransformationComponent* tr, RenderComponent* rd,
            const Heightfield& heightfield);

  private:
    float m_textureStrech = 10.0;
};

#endif // TERRAIN_H
