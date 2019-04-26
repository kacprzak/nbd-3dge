#include "Material.h"

namespace gfx {

void Material::applyTo(ShaderProgram* shaderProgram) const
{
    shaderProgram->setUniform("material.baseColorFactor", baseColorFactor);
    if (auto& t = textures[TextureUnit::BaseColor]) {
        shaderProgram->setUniform("baseColorSampler", TextureUnit::BaseColor);
        t->bind(TextureUnit::BaseColor);
    }

    shaderProgram->setUniform("material.normalScale", normalScale);
    if (auto& t = textures[TextureUnit::Normal]) {
        shaderProgram->setUniform("normalSampler", TextureUnit::Normal);
        t->bind(TextureUnit::Normal);
    }

    shaderProgram->setUniform("material.metallicFactor", metallicFactor);
    shaderProgram->setUniform("material.roughnessFactor", roughnessFactor);
    if (auto& t = textures[TextureUnit::MetallicRoughness]) {
        shaderProgram->setUniform("metallicRoughnessSampler", TextureUnit::MetallicRoughness);
        t->bind(TextureUnit::MetallicRoughness);
    }

    if (auto& t = textures[TextureUnit::Occlusion]) {
        shaderProgram->setUniform("occlusionStrength", occlusionStrength);
        shaderProgram->setUniform("occlusionSampler", TextureUnit::Occlusion);
        t->bind(TextureUnit::Occlusion);
    }

    shaderProgram->setUniform("material.emissiveFactor", emissiveFactor);
    if (auto& t = textures[TextureUnit::Emissive]) {
        shaderProgram->setUniform("emissiveSampler", TextureUnit::Emissive);
        t->bind(TextureUnit::Emissive);
    }
}

} // namespace gfx
