#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <algorithm>

Camera::Camera(int actorId, TransformationComponent* tr, RenderComponent* rd, glm::ivec2 windowSize)
    : RenderNode{actorId, tr, rd}
    , m_windowSize{windowSize}
    , m_ratio{m_windowSize.x / float(m_windowSize.y)}
{
    setPerspective();
}

Camera::Camera(glm::ivec2 windowSize)
    : Camera{-1, new TransformationComponent, nullptr, windowSize}
{
    m_ownsTransformation = true;
}

Camera::~Camera()
{
    if (m_ownsTransformation) delete transformation();
}

void Camera::setPerspective()
{
    m_projectionMatrix = glm::perspective(m_fov, m_ratio, m_near, m_far);
    m_frustum          = perspectiveArgsToFrustum(m_fov, m_ratio, m_near, m_far);

    for (int i = 0; i < s_shadowCascadesMax; ++i) {
        auto nearFar                 = cascadeIdx2NearFar(i);
        m_cascadeProjectionMatrix[i] = glm::perspective(m_fov, m_ratio, nearFar.x, nearFar.y);
    }
}

void Camera::setOrtho()
{
    float w2 = m_windowSize.x / 2.f;
    float h2 = m_windowSize.y / 2.f;

    m_projectionMatrix = glm::ortho(-w2, w2, -h2, h2, m_near, m_far);
}

void Camera::setOrtho(const Aabb& aabb)
{
    m_projectionMatrix = glm::ortho(aabb.minimum.x, aabb.maximum.x, aabb.minimum.y, aabb.maximum.y,
                                    -aabb.maximum.z, -aabb.minimum.z);

    m_frustum = aabb.toPositions();
}

void Camera::setWindowSize(glm::ivec2 size)
{
    m_windowSize = size;
    m_ratio      = m_windowSize.x / float(m_windowSize.y);

    if (m_perspective)
        setPerspective();
    else
        setOrtho();
}

//------------------------------------------------------------------------------

const glm::mat4& Camera::viewMatrix() const { return m_viewMatrix; }

const glm::mat4& Camera::projectionMatrix() const { return m_projectionMatrix; }

//------------------------------------------------------------------------------

void Camera::update(float delta)
{
    RenderNode::update(delta);

    glm::quat orien = transformation()->orientation;

    const auto T    = glm::translate(glm::mat4(1.f), -transformation()->position);
    const auto R    = glm::toMat4(glm::inverse(orien));
    m_viewMatrix    = R * T;
    m_viewMatrixInv = glm::inverse(m_viewMatrix);
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

//------------------------------------------------------------------------------

glm::vec2 Camera::cascadeIdx2NearFar(int cascadeIndex) const
{
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
    auto nearFar = cascadeIdx2NearFar(cascadeIndex);

    std::array<glm::vec4, 8> ans = perspectiveArgsToFrustum(m_fov, m_ratio, nearFar.x, nearFar.y);

    std::transform(std::cbegin(ans), std::cend(ans), std::begin(ans),
                   [this](auto p) { return m_viewMatrixInv * p; });
    return ans;
}

const glm::mat4& Camera::projectionMatrix(int cascadeIndex) const
{
    return m_cascadeProjectionMatrix[cascadeIndex];
}
