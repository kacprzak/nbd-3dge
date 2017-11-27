#ifndef CAMERA_H
#define CAMERA_H

#include "RenderNode.h"
#include <glm/glm.hpp>

class Camera : public RenderNode
{
  public:
    Camera(int actorId, TransformationComponent* tr, RenderComponent* rd, glm::ivec2 windowSize);
    Camera(glm::ivec2 windowSize);
    ~Camera();

    void draw(const Camera* /*c*/, const std::array<Light*, 8>& /*lights*/,
              Texture* /*environment*/) const override
    {
    }

    void update(float delta) override;
    void setPerspective();
    void setOrtho();
    void setOrtho(const Aabb& aabb);

    void setWindowSize(glm::ivec2 size);

    const glm::mat4& viewMatrix() const;
    const glm::mat4& projectionMatrix() const;

    bool isVisible(const Aabb& aabb) const;

    //! Returns view frustum in world space.
    std::array<glm::vec4, 8> frustum() const;

  private:
    glm::mat4 m_projectionMatrix;
    glm::mat4 m_viewMatrix;
    glm::mat4 m_viewMatrixInv;
    glm::ivec2 m_windowSize;
    std::array<glm::vec4, 8> m_frustum;
    bool m_perspective        = true;
    bool m_ownsTransformation = false;
};

#endif // CAMERA_H
