#include "Light.h"

#include "Material.h"

#include <glm/glm.hpp>

#include <algorithm>

// glm::vec4 Light::position() const
// {
//     if (m_lt->type == LightComponent::Type::Directional) {
//         return glm::normalize(transformation()->rotation * glm::vec4{0.f, 0.f, -1.0f, 0.0f});
//     } else {
//         return glm::vec4(transformation()->translation, 1.0f);
//     }
// }

void Light::applyTo(ShaderProgram* shaderProgram, int idx) const
{
    const auto& lightIdx = "lights[" + std::to_string(idx) + "]";
    shaderProgram->setUniform((lightIdx + ".position").c_str(), worldTranslation());
    shaderProgram->setUniform((lightIdx + ".ambient").c_str(), m_md.ambient);
    shaderProgram->setUniform((lightIdx + ".diffuse").c_str(), m_md.diffuse);
    shaderProgram->setUniform((lightIdx + ".specular").c_str(), m_md.specular);
}
