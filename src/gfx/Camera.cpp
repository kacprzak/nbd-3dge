#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <algorithm>

namespace gfx {

Camera::Camera() { setPerspective(); }

//------------------------------------------------------------------------------

void Camera::drawFrustum(ShaderProgram* shaderProgram, const Camera* camera) const
{
    shaderProgram->use();

    shaderProgram->setUniform("projectionMatrix", camera->projectionMatrix());
    shaderProgram->setUniform("viewMatrix", camera->viewMatrix());
    shaderProgram->setUniform("modelMatrix", m_viewMatrixInv);

    for (int i = 0; i < s_shadowCascadesMax; ++i) {

        for (std::size_t j = 0; j < m_cascadeFrustums[i].size(); ++j) {
            const auto uniformName = "corners[" + std::to_string(j) + "]";
            shaderProgram->setUniform(uniformName.c_str(), m_cascadeFrustums[i][j]);
        }

        glDrawArrays(GL_POINTS, 0, 1);
    }
}

//------------------------------------------------------------------------------

void Camera::setPerspective(bool updateCascades)
{
    m_projectionMatrix = glm::perspective(m_fov, m_ratio, m_near, m_far);
    m_frustum          = perspectiveArgsToFrustum(m_fov, m_ratio, m_near, m_far);

    if (updateCascades) {
        for (int i = 0; i < s_shadowCascadesMax; ++i) {
            auto nearFar = cascadeIdx2NearFar(i);

            m_cascadeFrustums[i] = perspectiveArgsToFrustum(m_fov, m_ratio, nearFar.x, nearFar.y);
            m_cascadeProjectionMatrix[i] = glm::perspective(m_fov, m_ratio, nearFar.x, nearFar.y);
        }
    }

    m_perspective = true;
}

void Camera::setPerspective(float fov, float near, float far, bool updateCascades)
{
    assert(near < far);

    m_fov  = fov;
    m_near = near;
    m_far  = far;

    setPerspective(updateCascades);
    m_perspective = true;
}

void Camera::setOrtho(bool updateCascades)
{
    m_projectionMatrix = glm::ortho(m_left, m_right, m_bottom, m_top, -m_far, -m_near);
    m_frustum          = ortoArgsToFrustum(m_left, m_right, m_bottom, m_top, m_near, m_far);

    if (updateCascades) {
        for (int i = 0; i < s_shadowCascadesMax; ++i) {
            auto nearFar = cascadeIdx2NearFar(i);

            m_cascadeFrustums[i] =
                ortoArgsToFrustum(m_left, m_right, m_bottom, m_top, nearFar.x, nearFar.y);
            m_cascadeProjectionMatrix[i] =
                glm::ortho(m_left, m_right, m_bottom, m_top, -nearFar.y, -nearFar.x);
        }
    }

    m_perspective = false;
}

void Camera::setOrtho(const Aabb& aabb, bool updateCascades)
{
    m_left   = aabb.minimum.x;
    m_right  = aabb.maximum.x;
    m_bottom = aabb.minimum.y;
    m_top    = aabb.maximum.y;
    m_near   = aabb.minimum.z;
    m_far    = aabb.maximum.z;

    assert(m_near < m_far);

    setOrtho(updateCascades);
}

void Camera::setOrtho(int cascadeIndex, const Aabb& aabb)
{
    const float left   = aabb.minimum.x;
    const float right  = aabb.maximum.x;
    const float bottom = aabb.minimum.y;
    const float top    = aabb.maximum.y;
    const float near   = aabb.minimum.z;
    const float far    = aabb.maximum.z;

    assert(near < far);

    m_cascadeFrustums[cascadeIndex] = ortoArgsToFrustum(left, right, bottom, top, near, far);
    m_cascadeProjectionMatrix[cascadeIndex] = glm::ortho(left, right, bottom, top, -far, -near);
}

void Camera::setAspectRatio(float ratio)
{
    m_ratio = ratio;

    if (m_perspective)
        setPerspective();
    else
        setOrtho();
}

//------------------------------------------------------------------------------

const glm::mat4& Camera::viewMatrix() const { return m_viewMatrix; }

const glm::mat4& Camera::projectionMatrix() const { return m_projectionMatrix; }

//------------------------------------------------------------------------------

void Camera::update(const glm::mat4& parentModelMatrix, float /*delta*/)
{
    m_viewMatrixInv = parentModelMatrix;
    m_viewMatrix    = glm::inverse(parentModelMatrix);
}

//------------------------------------------------------------------------------

bool Camera::isVisible(const Aabb& aabb) const
{
    const Aabb& homogeneous = m_projectionMatrix * m_viewMatrix * aabb;
    return Aabb::unit().intersects(homogeneous);
}

//------------------------------------------------------------------------------

Camera::Frustum Camera::perspectiveArgsToFrustum(float fov, float ratio, float near,
                                                 float far) const
{
    const glm::vec4 up    = {0.f, 1.f, 0.f, 0.f};
    const glm::vec4 right = {1.f, 0.f, 0.f, 0.f};

    float nearHeight = 2.f * tan(fov / 2.f) * near;
    float nearWidth  = nearHeight * ratio;

    float farHeight = 2.f * tan(fov / 2.f) * far;
    float farWidth  = farHeight * ratio;

    glm::vec4 fc = {0.f, 0.f, -far, 1.f};

    glm::vec4 ftl = fc + (up * farHeight / 2.f) - (right * farWidth / 2.f);
    glm::vec4 ftr = fc + (up * farHeight / 2.f) + (right * farWidth / 2.f);
    glm::vec4 fbl = fc - (up * farHeight / 2.f) - (right * farWidth / 2.f);
    glm::vec4 fbr = fc - (up * farHeight / 2.f) + (right * farWidth / 2.f);

    glm::vec4 nc = {0.f, 0.f, -near, 1.f};

    glm::vec4 ntl = nc + (up * nearHeight / 2.f) - (right * nearWidth / 2.f);
    glm::vec4 ntr = nc + (up * nearHeight / 2.f) + (right * nearWidth / 2.f);
    glm::vec4 nbl = nc - (up * nearHeight / 2.f) - (right * nearWidth / 2.f);
    glm::vec4 nbr = nc - (up * nearHeight / 2.f) + (right * nearWidth / 2.f);

    return {{nbl, nbr, ntr, ntl, ftr, fbr, fbl, ftl}};
}

Camera::Frustum Camera::ortoArgsToFrustum(float left, float right, float bottom, float top,
                                          float near, float far) const
{
    glm::vec4 ftl{left, top, far, 1.f};
    glm::vec4 ftr{right, top, far, 1.f};
    glm::vec4 fbl{left, bottom, far, 1.f};
    glm::vec4 fbr{right, bottom, far, 1.f};

    glm::vec4 ntl{left, top, near, 1.f};
    glm::vec4 ntr{right, top, near, 1.f};
    glm::vec4 nbl{left, bottom, near, 1.f};
    glm::vec4 nbr{right, bottom, near, 1.f};

    return {{nbl, nbr, ntr, ntl, ftr, fbr, fbl, ftl}};
}

//------------------------------------------------------------------------------

glm::vec2 Camera::cascadeIdx2NearFar(int cascadeIndex) const
{
    //#define TEST
#ifdef TEST
    // For debug
    float far[5] = {1.f, 50.f, 200.f, 500.f, 1000.f};
    return {far[cascadeIndex], far[cascadeIndex + 1]};
#else

    // http://developer.download.nvidia.com/SDK/10.5/opengl/src/cascaded_shadow_maps/doc/cascaded_shadow_maps.pdf
    float near = (cascadeIndex == 0)
                     ? m_near
                     : m_near * std::pow(m_far / m_near, cascadeIndex / float(s_shadowCascadesMax));
    float far =
        (cascadeIndex == s_shadowCascadesMax - 1)
            ? m_far
            : m_near * std::pow(m_far / m_near, (cascadeIndex + 1) / float(s_shadowCascadesMax));
    ;
    return {near, far};
#endif
}

void Camera::setCascade(int cascadeIndex)
{
    if (cascadeIndex >= 0 && cascadeIndex < s_shadowCascadesMax) {
        m_frustum          = m_cascadeFrustums[cascadeIndex];
        m_projectionMatrix = m_cascadeProjectionMatrix[cascadeIndex];
    } else {
        if (m_perspective)
            setPerspective(false);
        else
            setOrtho(false);
    }
}

//------------------------------------------------------------------------------

Camera::Frustum Camera::frustum() const
{
    std::array<glm::vec4, 8> ans;
    std::transform(std::cbegin(m_frustum), std::cend(m_frustum), std::begin(ans),
                   [this](auto p) { return m_viewMatrixInv * p; });
    return ans;
}

Camera::Frustum Camera::frustum(int cascadeIndex) const
{
    assert(cascadeIndex < s_shadowCascadesMax);

    std::array<glm::vec4, 8> ans = m_cascadeFrustums[cascadeIndex];

    std::transform(std::cbegin(ans), std::cend(ans), std::begin(ans),
                   [this](auto p) { return m_viewMatrixInv * p; });
    return ans;
}

//------------------------------------------------------------------------------

const glm::mat4& Camera::projectionMatrix(int cascadeIndex) const
{
    return m_cascadeProjectionMatrix[cascadeIndex];
}

//------------------------------------------------------------------------------

void Camera::applyTo(ShaderProgram* shaderProgram) const
{
    shaderProgram->setUniform("projectionMatrix", projectionMatrix());
    shaderProgram->setUniform("viewMatrix", viewMatrix());
    shaderProgram->setUniform("cameraPosition", worldTranslation());
}

} // namespace gfx
