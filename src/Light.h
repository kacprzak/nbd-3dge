#ifndef LIGHT_H
#define LIGHT_H

#include "Camera.h"

#include <glm/fwd.hpp>

class Light : public Camera
{
  public:
    Light(int actorId, TransformationComponent* tr, RenderComponent* rd, LightComponent* lt)
        : Camera{}
        , m_lt{lt}
    {
        if (lt->type == LightComponent::Type::Directional) setOrtho();
    }

    void applyTo(ShaderProgram* shaderProgram, int idx) const;

  private:
    LightComponent* m_lt = nullptr;
    MaterialData m_md;
};

#endif /* LIGHT_H */
