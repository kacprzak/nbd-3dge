#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

Camera::Camera(int actorId, TransformationComponent* tr, RenderComponent* rd,
               Texture::Size windowSize)
    : RenderNode{actorId, tr, rd}
    , m_windowSize{windowSize}
{
    setPerspective();
}

Camera::Camera(Texture::Size windowSize)
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
    float ratio        = m_windowSize.w / float(m_windowSize.h);
    m_projectionMatrix = glm::perspective(45.f, ratio, 1.f, 1200.f);
}

void Camera::setOrtho()
{
    m_projectionMatrix = glm::ortho(-100.f, 100.f, -100.f, 100.f, 1.f, 1200.f);
}

void Camera::setWindowSize(Texture::Size size)
{
    m_windowSize = size;
    if (m_perspective)
        setPerspective();
    else
        setOrtho();
}

const glm::mat4& Camera::viewMatrix() const { return m_viewMatrix; }

const glm::mat4& Camera::projectionMatrix() const { return m_projectionMatrix; }

void Camera::update(float delta)
{
    RenderNode::update(delta);

    glm::quat orien = transformation()->orientation;

    const auto T = glm::translate(glm::mat4(1.f), -transformation()->position);
    const auto R = glm::toMat4(glm::inverse(orien));
    m_viewMatrix = R * T;
}
