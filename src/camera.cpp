#include "camera.h"

#include <glm/gtc/matrix_transform.hpp>

void Camera::setPerspective(float angle, float ratio, float near, float far)
{
    m_projectionMatrix = glm::perspective(angle, ratio, near, far);
}

const glm::mat4& Camera::viewMatrix() const
{
    return modelMatrix();
}

const glm::mat4& Camera::projectionMatrix() const
{
    return m_projectionMatrix;
}
