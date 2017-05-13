#include "Terrain.h"

Terrain::Terrain(int actorId, TransformationComponent* tr, RenderComponent* rd,
                 const Heightfield& heightfield)
    : RenderNode{actorId, tr, rd}
{
    auto mesh = std::shared_ptr<Mesh>{
        Mesh::fromHeightmap(heightfield.heights, heightfield.w, heightfield.h, m_textureStrech)};
    setMesh(mesh);
}
