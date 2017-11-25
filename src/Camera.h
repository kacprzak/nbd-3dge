#ifndef CAMERA_H
#define CAMERA_H

#include "RenderNode.h"
#include <glm/glm.hpp>

class Camera : public RenderNode
{
  public:
    Camera(int actorId, TransformationComponent* tr, RenderComponent* rd, Texture::Size windowSize);
    Camera(Texture::Size windowSize);
    ~Camera();

    void draw(const Camera* /*c*/, const std::array<Light*, 8>& /*lights*/,
              Texture* /*environment*/) const override
    {
    }

    void update(float delta) override;
    void setPerspective();
    void setOrtho();
    void setOrtho(const Aabb& aabb);

    void setWindowSize(Texture::Size size);

    const glm::mat4& viewMatrix() const;
    const glm::mat4& projectionMatrix() const;

    bool isVisible(const Aabb& aabb) const;

  private:
    glm::mat4 m_projectionMatrix;
    glm::mat4 m_viewMatrix;
    Texture::Size m_windowSize;
    bool m_perspective        = true;
    bool m_ownsTransformation = false;
};

#endif // CAMERA_H
