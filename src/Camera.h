#ifndef CAMERA_H
#define CAMERA_H

#include "RenderNode.h"
#include <glm/glm.hpp>

class Camera : public RenderNode
{
  protected:
    using Frustum = std::array<glm::vec4, 8>;

  public:
    Camera(int actorId, TransformationComponent* tr, RenderComponent* rd, glm::ivec2 windowSize);
    Camera(glm::ivec2 windowSize);
    ~Camera();

    float near() const { return m_near; }
    float far() const { return m_far; }

    void draw(const Camera* /*c*/, const std::array<Light*, 8>& /*lights*/,
              Texture* /*environment*/) const override
    {
    }

    void drawFrustum(ShaderProgram* shaderProgram, const Camera* camera);

    void update(float delta) override;
    void setPerspective();
    void setPerspective(float fov, float near, float far);
    void setOrtho();
    void setOrtho(const Aabb& aabb);

    void setWindowSize(glm::ivec2 size);

    const glm::mat4& viewMatrix() const;
    const glm::mat4& projectionMatrix() const;
    const glm::mat4& projectionMatrix(int cascadeIndex) const;

    bool isVisible(const Aabb& aabb) const;

    //! Returns view frustum in world space.
    Frustum frustum() const;
    Frustum frustum(int cascadeIndex) const;

    static const int s_shadowCascadesMax = 4;

    glm::vec2 cascadeIdx2NearFar(int cascadeIndex) const;

  protected:
    Frustum perspectiveArgsToFrustum(float fov, float ratio, float near, float far) const;
    Frustum ortoArgsToFrustum(float left, float right, float bottom, float top, float near,
                              float far) const;

    glm::ivec2 m_windowSize;
    float m_near   = 1.f;
    float m_far    = 1000.f;
    float m_fov    = 45.f;
    float m_ratio  = 1.f;
    float m_left   = -100.f;
    float m_right  = 100.f;
    float m_bottom = -100.f;
    float m_top    = 100.f;

    glm::mat4 m_projectionMatrix;
    glm::mat4 m_viewMatrix;
    glm::mat4 m_viewMatrixInv;
    glm::mat4 m_cascadeProjectionMatrix[s_shadowCascadesMax];

    std::array<glm::vec4, 8> m_frustum;
    bool m_perspective        = true;
    bool m_ownsTransformation = false;
};

#endif // CAMERA_H
