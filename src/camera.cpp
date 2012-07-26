#include "camera.h"

#include <glm/gtc/matrix_transform.hpp>

void Camera::setPerspective(float angle, float ratio, float near, float far)
{
    m_projectionMatrix = glm::perspective(angle, ratio, near, far);
}

const glm::mat4& Camera::viewMatrix()
{
    m_viewMatrix = glm::mat4(1.0f);
    glm::vec3 orien = orientation();

    m_viewMatrix = glm::rotate(m_viewMatrix, -orien.x, glm::vec3(1.0f, 0.0f, 0.0f));
    m_viewMatrix = glm::rotate(m_viewMatrix, -orien.y, glm::vec3(0.0f, 1.0f, 0.0f));
    m_viewMatrix = glm::rotate(m_viewMatrix, -orien.z, glm::vec3(0.0f, 0.0f, 1.0f));

    m_viewMatrix = glm::translate(m_viewMatrix, -position());

    return m_viewMatrix;
}

const glm::mat4& Camera::projectionMatrix() const
{
    return m_projectionMatrix;
}
