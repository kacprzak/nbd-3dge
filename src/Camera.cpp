#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

Camera::Camera()
    : RenderNode{-1, new TransformationComponent, nullptr}
{
    float ratio = 800 / float(600);
    setPerspective(45.0f, ratio, 1.0f, 1200.0f);
}

Camera::~Camera() { delete transformation(); }

void Camera::setPerspective(float angle, float ratio, float near, float far)
{
    m_projectionMatrix = glm::perspective(angle, ratio, near, far);
}

void Camera::rotate(float yaw, float pitch, float /*roll*/)
{
    m_yawPitch += glm::vec2{glm::radians(-yaw), glm::radians(-pitch)};
    m_yawPitch.x = glm::mod(m_yawPitch.x, glm::two_pi<float>());
    m_yawPitch.y = glm::mod(m_yawPitch.y, glm::two_pi<float>());

    auto& orient = transformation()->orientation;
    orient       = glm::angleAxis(m_yawPitch.x, glm::vec3(0, 1, 0));
    orient *= glm::angleAxis(m_yawPitch.y, glm::vec3(1, 0, 0));
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
