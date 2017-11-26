#ifndef LIGHT_H
#define LIGHT_H

#include "Camera.h"

#include <glm/fwd.hpp>

class Light : public Camera
{
  public:
    Light(int actorId, TransformationComponent* tr, RenderComponent* rd, LightComponent* lt,
          glm::ivec2 windowSize)
        : Camera{actorId, tr, rd, windowSize}
        , m_lt{lt}
    {
        if (lt->type == LightComponent::Type::Directional) setOrtho();
    }

    glm::vec4 position() const;
    glm::vec3 ambient() const;
    glm::vec3 diffuse() const;
    glm::vec3 specular() const;

  private:
    LightComponent* m_lt = nullptr;
};

#endif /* LIGHT_H */
