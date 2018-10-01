#include "Material.h"

void Material::applyTo(ShaderProgram* shaderProgram) const
{
    shaderProgram->setUniform("material.baseColorFactor", baseColorFactor);
    if (textures[Unit::BaseColor]) {
        shaderProgram->setUniform("baseColorSampler", Unit::BaseColor);
        textures[Unit::BaseColor]->bind(Unit::BaseColor);
    }

    shaderProgram->setUniform("material.normalScale", normalScale);
    if (textures[Unit::Normal]) {
        shaderProgram->setUniform("normalSampler", Unit::Normal);
        textures[Unit::Normal]->bind(Unit::Normal);
    }

    shaderProgram->setUniform("material.metallicFactor", metallicFactor);
    shaderProgram->setUniform("material.roughnessFactor", roughnessFactor);
    if (textures[Unit::MetallicRoughness]) {
        shaderProgram->setUniform("metallicRoughnessSampler", Unit::MetallicRoughness);
        textures[Unit::MetallicRoughness]->bind(Unit::MetallicRoughness);
    }

    if (textures[Unit::Occlusion]) {
        shaderProgram->setUniform("occlusionStrength", occlusionStrength);
        shaderProgram->setUniform("occlusionSampler", Unit::Occlusion);
        textures[Unit::Occlusion]->bind(Unit::Occlusion);
    }

    shaderProgram->setUniform("material.emissiveFactor", emissiveFactor);
    if (textures[Unit::Emissive]) {
        shaderProgram->setUniform("emissiveSampler", Unit::Emissive);
        textures[Unit::Emissive]->bind(Unit::Emissive);
    }
}
