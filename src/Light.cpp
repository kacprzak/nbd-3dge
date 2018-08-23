#include "Light.h"

#include "Material.h"

#include <glm/glm.hpp>

#include <algorithm>

glm::vec4 Light::position() const
{
    /*
    if (m_lt->type == LightComponent::Type::Directional) {
        return glm::normalize(transformation()->rotation * glm::vec4{0.f, 0.f, -1.0f, 0.0f});
    } else {
        return glm::vec4(transformation()->translation, 1.0f);
    }
    */
    return {};
}

glm::vec3 Light::ambient() const { return m_material.ambient(); }

glm::vec3 Light::diffuse() const { return m_material.diffuse(); }

glm::vec3 Light::specular() const { return m_material.specular(); }
