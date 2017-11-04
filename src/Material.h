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

    glm::vec3 ambient() const { return md.ambient; }
    glm::vec3 diffuse() const { return md.diffuse; }
    glm::vec3 specular() const { return md.specular; }
    glm::vec3 emission() const { return md.emission; }
    float shininess() const { return md.shininess; }

    // private:
    MaterialData md;
    std::vector<std::shared_ptr<Texture>> textures;
};

#endif /* MATERIAL_H */
