#ifndef MATERIAL_H
#define MATERIAL_H

#include "MaterialData.h"
#include "Texture.h"

#include <vector>

class Material
{
  public:
    Material(const MaterialData& md, std::vector<std::shared_ptr<Texture>> textures)
        : md{md}
        , textures{textures}
    {
    }

    // private:
    MaterialData md;
    std::vector<std::shared_ptr<Texture>> textures;
};

#endif /* MATERIAL_H */
