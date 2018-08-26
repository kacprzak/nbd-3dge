#ifndef MATERIAL_H
#define MATERIAL_H

#include "MaterialData.h"
#include "ShaderProgram.h"
#include "Texture.h"

#include <vector>

class Material
{
  public:
    Material(const MaterialData& md, std::vector<std::shared_ptr<Texture>> textures)
    //       : md{md}
    //    , textures{textures}
    {
        if (textures.size() > 0) {
            baseColorTexture = textures.at(0);
        }
    }

    Material() = default;

    void applyTo(ShaderProgram* shaderProgram) const;

    // private:
    // MaterialData md;
    // std::vector<std::shared_ptr<Texture>> textures;
    std::string name;

    enum AlphaMode { Opaque, Mask, Blend };
    enum Unit { BaseColor, Normal, Occlusion, Emissive, MetallicRoughness, Environment = 7 };

    AlphaMode alphaMode = Opaque;
    float alphaCutoff   = 0.5f;

    bool doubleSided = false;

    glm::vec4 baseColorFactor{0.5f, 0.5f, 0.5f, 1.0f};
    std::shared_ptr<Texture> baseColorTexture;

    float metallicFactor  = 0.0f;
    float roughnessFactor = 0.5f;
    std::shared_ptr<Texture> metallicRoughnessTexture;

    float normalScale = 1.0;
    std::shared_ptr<Texture> normalTexture;

    std::shared_ptr<Texture> occlusionTexture;

    glm::vec3 emissiveFactor{0.0f, 0.0f, 0.0f};
    std::shared_ptr<Texture> emissiveTexture;
};

#endif /* MATERIAL_H */
