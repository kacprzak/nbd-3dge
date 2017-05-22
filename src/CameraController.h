#ifndef CAMERACONTROLLER_H
#define CAMERACONTROLLER_H

#include <glm/glm.hpp>

class CameraController
{
  public:
    virtual ~CameraController() = default;

    virtual void update(float /*deltaTime*/) = 0;

    TransformationComponent* camera = nullptr;
    TransformationComponent* player = nullptr;
    ControlComponent cameraActions;
};

//------------------------------------------------------------------------------

class FreeCamera : public CameraController
{
  private:
    glm::vec2 m_yawPitch;

    void rotateCamera(float yaw, float pitch, float /*roll*/)
    {
        m_yawPitch += glm::vec2{glm::radians(-yaw), glm::radians(-pitch)};
        m_yawPitch.x = glm::mod(m_yawPitch.x, glm::two_pi<float>());
        m_yawPitch.y = glm::mod(m_yawPitch.y, glm::two_pi<float>());

        auto& orient = camera->orientation;
        orient       = glm::angleAxis(m_yawPitch.x, glm::vec3(0, 1, 0));
        orient *= glm::angleAxis(m_yawPitch.y, glm::vec3(1, 0, 0));
    }

  public:
    float m_cameraSpeed = 50.0f;

    void update(float deltaTime)
    {
        float cameraSpeedMultiplyer = 0.5f;

        if (cameraActions.actions & ControlComponent::Fire) cameraSpeedMultiplyer = 5.0f;

        auto distance = deltaTime * m_cameraSpeed * cameraSpeedMultiplyer;

        rotateCamera(cameraActions.axes.x, cameraActions.axes.y, cameraActions.axes.z);

        if (cameraActions.actions & ControlComponent::Forward) {
            const auto orien = camera->orientation;
            const auto delta = glm::rotate(orien, glm::vec4{0.f, 0.f, -distance, 0.f});
            camera->position += glm::vec3{delta};
        }

        if (cameraActions.actions & ControlComponent::Back) {
            const auto orien = camera->orientation;
            const auto delta = glm::rotate(orien, glm::vec4{0.f, 0.f, distance, 0.f});
            camera->position += glm::vec3{delta};
        }

        if (cameraActions.actions & ControlComponent::StrafeRight) {
            const auto orien = camera->orientation;
            const auto delta = glm::rotate(orien, glm::vec4{distance, 0.f, 0.f, 0.f});
            camera->position += glm::vec3{delta};
        }

        if (cameraActions.actions & ControlComponent::StrafeLeft) {
            const auto orien = camera->orientation;
            const auto delta = glm::rotate(orien, glm::vec4{-distance, 0.f, 0.f, 0.f});
            camera->position += glm::vec3{delta};
        }

        if (cameraActions.actions & ControlComponent::Up) {
            const auto orien = camera->orientation;
            const auto delta = glm::rotate(orien, glm::vec4{0.f, distance, 0.f, 0.f});
            camera->position += glm::vec3{delta};
        }

        if (cameraActions.actions & ControlComponent::Down) {
            const auto orien = camera->orientation;
            const auto delta = glm::rotate(orien, glm::vec4{0.f, -distance, 0.f, 0.f});
            camera->position += glm::vec3{delta};
        }
    }
};

//------------------------------------------------------------------------------

class TppCamera : public CameraController
{
  public:
    void update(float /*deltaTime*/)
    {
        if (!player) return;

        // Fixed position relative to player
        auto orien          = player->orientation;
        const auto delta    = glm::rotate(orien, glm::vec4{0.f, 1.5f, -6.f, 0.f});
        camera->position    = player->position + glm::vec3{delta};
        camera->orientation = orien * glm::quat{0.f, 0.f, 1.f, 0.f};
    }
};

#endif /* CAMERACONTROLLER_H */
