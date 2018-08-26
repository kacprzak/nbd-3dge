#include "Material.h"

void Material::applyTo(ShaderProgram* shaderProgram) const
{
    /*
    int textureUnit = 0;

    for (const auto& texture : textures) {
        const std::string& name = "sampler" + std::to_string(textureUnit);
        shaderProgram->setUniform(name.c_str(), textureUnit);

        texture->bind(textureUnit++);
    }

    shaderProgram->setUniform("material.ambient", md.ambient);
    shaderProgram->setUniform("material.diffuse", md.diffuse);
    shaderProgram->setUniform("material.specular", md.specular);
    shaderProgram->setUniform("material.emission", md.emission);
    shaderProgram->setUniform("material.shininess", md.shininess);
    */

    shaderProgram->setUniform("material.baseColorFactor", baseColorFactor);
    if (baseColorTexture) {
        shaderProgram->setUniform("baseColorSampler", Unit::BaseColor);
        baseColorTexture->bind(Unit::BaseColor);
    }

    shaderProgram->setUniform("material.normalScale", normalScale);
    if (normalTexture) {
        shaderProgram->setUniform("normalSampler", Unit::Normal);
        normalTexture->bind(Unit::BaseColor);
    }
}
