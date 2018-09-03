#ifndef LIGHT_H
#define LIGHT_H

#include "Camera.h"

#include <glm/fwd.hpp>

class Light : public Camera
{
  public:
    Light(int actorId, LightComponent* lt)
        : Camera{}
        , m_lt{lt}
    {
        if (lt->type == LightComponent::Type::Directional) setOrtho();
    }

    void applyTo(ShaderProgram* shaderProgram, int idx) const;

  private:
    LightComponent* m_lt = nullptr;
    glm::vec3 m_color{10.0f};
};

#endif /* LIGHT_H */
