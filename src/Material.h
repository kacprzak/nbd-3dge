#ifndef MATERIAL_H
#define MATERIAL_H

#include "MaterialData.h"
#include "ShaderProgram.h"
#include "Texture.h"

class Material final
{
  public:
    void applyTo(ShaderProgram* shaderProgram) const;

    std::string name;

    enum AlphaMode { Opaque, Mask, Blend };

    AlphaMode alphaMode = Opaque;
    float alphaCutoff   = 0.5f;
    bool doubleSided    = false;
    glm::vec4 baseColorFactor{0.5f, 0.5f, 0.5f, 1.0f};
    float metallicFactor    = 0.0f;
    float roughnessFactor   = 0.5f;
    float normalScale       = 1.0f;
    float occlusionStrength = 1.0f;
    glm::vec3 emissiveFactor{0.0f, 0.0f, 0.0f};

    TexturePack textures{};
};

#endif /* MATERIAL_H */
