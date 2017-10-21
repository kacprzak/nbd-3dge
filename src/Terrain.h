#ifndef TERRAIN_H
#define TERRAIN_H

#include "Heightfield.h"
#include "RenderNode.h"

class Terrain : public RenderNode
{
  public:
    Terrain(int actorId, TransformationComponent* tr, RenderComponent* rd,
            const Heightfield& heightfield);

  private:
    float m_textureStrech = 1.0;
};

#endif // TERRAIN_H
