#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <algorithm>

Camera::Camera(int actorId, TransformationComponent* tr, RenderComponent* rd, glm::ivec2 windowSize)
    : RenderNode{actorId, tr, rd}
    , m_windowSize{windowSize}
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
    float fov   = 45.f;
    float ratio = m_windowSize.x / float(m_windowSize.y);
    float near  = 1.f;
    float far   = 1200.f;

    m_projectionMatrix = glm::perspective(fov, ratio, near, far);

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

    m_frustum = {{nbl, nbr, ntr, ntl, ftr, fbr, fbl, ftl}};
}

void Camera::setOrtho()
{
    m_projectionMatrix = glm::ortho(-100.f, 100.f, -100.f, 100.f, 1.f, 1200.f);
}

void Camera::setOrtho(const Aabb& aabb)
{
    m_projectionMatrix =
        glm::ortho(aabb.left(), aabb.right(), aabb.bottom(), aabb.top(), -aabb.far(), -aabb.near());

    m_frustum = aabb.toPositions();
}

void Camera::setWindowSize(glm::ivec2 size)
{
    m_windowSize = size;
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

std::array<glm::vec4, 8> Camera::frustum() const
{
    std::array<glm::vec4, 8> ans;
    std::transform(std::cbegin(m_frustum), std::cend(m_frustum), std::begin(ans),
                   [this](auto p) { return m_viewMatrixInv * p; });
    return ans;
}
