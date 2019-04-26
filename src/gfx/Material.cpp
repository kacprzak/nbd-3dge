#include "Material.h"

namespace gfx {

void Material::applyTo(ShaderProgram* shaderProgram) const
{
    shaderProgram->setUniform("material.baseColorFactor", baseColorFactor);
    if (textures[TextureUnit::BaseColor]) {
        shaderProgram->setUniform("baseColorSampler", TextureUnit::BaseColor);
        textures[TextureUnit::BaseColor]->bind(TextureUnit::BaseColor);
    }

    shaderProgram->setUniform("material.normalScale", normalScale);
    if (textures[TextureUnit::Normal]) {
        shaderProgram->setUniform("normalSampler", TextureUnit::Normal);
        textures[TextureUnit::Normal]->bind(TextureUnit::Normal);
    }

    shaderProgram->setUniform("material.metallicFactor", metallicFactor);
    shaderProgram->setUniform("material.roughnessFactor", roughnessFactor);
    if (textures[TextureUnit::MetallicRoughness]) {
        shaderProgram->setUniform("metallicRoughnessSampler", TextureUnit::MetallicRoughness);
        textures[TextureUnit::MetallicRoughness]->bind(TextureUnit::MetallicRoughness);
    }

    if (textures[TextureUnit::Occlusion]) {
        shaderProgram->setUniform("occlusionStrength", occlusionStrength);
        shaderProgram->setUniform("occlusionSampler", TextureUnit::Occlusion);
        textures[TextureUnit::Occlusion]->bind(TextureUnit::Occlusion);
    }

    shaderProgram->setUniform("material.emissiveFactor", emissiveFactor);
    if (textures[TextureUnit::Emissive]) {
        shaderProgram->setUniform("emissiveSampler", TextureUnit::Emissive);
        textures[TextureUnit::Emissive]->bind(TextureUnit::Emissive);
    }
}

} // namespace gfx
