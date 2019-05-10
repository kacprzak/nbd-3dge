#ifndef CAMERA_H
#define CAMERA_H

#include "Aabb.h"
#include "ShaderProgram.h"

#include <glm/glm.hpp>

// minwindef.h defines those
#undef near
#undef far

namespace gfx {

class Camera
{
  protected:
    using Frustum = std::array<glm::vec4, 8>;

  public:
    Camera();
    Camera(Camera&& other) = default;

    float zNear() const { return m_near; }
    float zFar() const { return m_far; }

    void drawFrustum(ShaderProgram* shaderProgram, const Camera* camera) const;

    void update(const glm::mat4& parentModelMatrix, float delta);

    void setPerspective(bool updateCascades = true);
    void setPerspective(float fov, float near, float far, bool updateCascades = true);
    void setOrtho(bool updateCascades = true);
    void setOrtho(const Aabb& aabb, bool updateCascades = true);
    void setOrtho(int cascadeIndex, const Aabb& aabb);

    void setAspectRatio(float ratio);

    const glm::mat4& viewMatrix() const;
    const glm::mat4& projectionMatrix() const;
    const glm::mat4& projectionMatrix(int cascadeIndex) const;

    glm::vec4 worldTranslation() const { return glm::inverse(viewMatrix())[3]; }

    bool isVisible(const Aabb& aabb) const;

    //! Returns view frustum in world space.
    Frustum frustum() const;
    Frustum frustum(int cascadeIndex) const;

    static const int s_shadowCascadesMax = 4;

    glm::vec2 cascadeIdx2NearFar(int cascadeIndex) const;
    void setCascade(int cascadeIndex);

  protected:
    Frustum perspectiveArgsToFrustum(float fov, float ratio, float near, float far) const;
    Frustum ortoArgsToFrustum(float left, float right, float bottom, float top, float near,
                              float far) const;

    float m_near   = 0.005f;
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

    Frustum m_frustum;
    std::array<Frustum, s_shadowCascadesMax> m_cascadeFrustums;
    bool m_perspective = true;
};

} // namespace gfx

#endif // CAMERA_H
