#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera() : GfxNode{-1, new TransformationComponent}
{
    float ratio = 800 / float(600);
    setPerspective(45.0f, ratio, 1.0f, 1200.0f);
}

Camera::~Camera()
{
    delete transformation();
}

void Camera::setPerspective(float angle, float ratio, float near, float far)
{
    m_projectionMatrix = glm::perspective(angle, ratio, near, far);
}

const glm::mat4& Camera::viewMatrix() const
{
    return m_viewMatrix;
}

const glm::mat4& Camera::projectionMatrix() const
{
    return m_projectionMatrix;
}

void Camera::update(float delta)
{
    GfxNode::update(delta);

    m_viewMatrix = glm::mat4(1.0f);
    glm::vec4 orien = transformation()->orientation;

    m_viewMatrix = glm::rotate(m_viewMatrix, -orien.x, glm::vec3(1.0f, 0.0f, 0.0f));
    m_viewMatrix = glm::rotate(m_viewMatrix, -orien.y, glm::vec3(0.0f, 1.0f, 0.0f));
    m_viewMatrix = glm::rotate(m_viewMatrix, -orien.z, glm::vec3(0.0f, 0.0f, 1.0f));

    m_viewMatrix = glm::translate(m_viewMatrix, -(transformation()->position));
}
