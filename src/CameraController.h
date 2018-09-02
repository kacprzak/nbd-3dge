#ifndef CAMERACONTROLLER_H
#define CAMERACONTROLLER_H

#include "Components.h"
#include "Script.h"

#include <glm/glm.hpp>

class CameraController : public Script
{
  public:
    TransformationComponent* camera = nullptr;
    TransformationComponent* player = nullptr;
    ControlComponent cameraActions;
};

//------------------------------------------------------------------------------

class FreeCameraController : public CameraController
{
  private:
    void rotateCamera(float yaw, float pitch, float /*roll*/)
    {
        static glm::vec2 yawPitch;

        yawPitch += glm::vec2{glm::radians(-yaw), glm::radians(-pitch)};
        yawPitch.x = glm::mod(yawPitch.x, glm::two_pi<float>());
        yawPitch.y = glm::mod(yawPitch.y, glm::two_pi<float>());

        auto& orient = camera->rotation;
        orient       = glm::angleAxis(yawPitch.x, glm::vec3(0, 1, 0));
        orient *= glm::angleAxis(yawPitch.y, glm::vec3(1, 0, 0));
    }

  public:
    float m_cameraSpeed = 5.0f;

    void execute(float deltaTime, Actor* /*actor*/) override
    {
        float cameraSpeedMultiplyer = 0.01f;

        if (cameraActions.actions & ControlComponent::Fire) cameraSpeedMultiplyer = 5.0f;

        auto distance = deltaTime * m_cameraSpeed * cameraSpeedMultiplyer;

        rotateCamera(cameraActions.axes.x, cameraActions.axes.y, cameraActions.axes.z);

        if (cameraActions.actions & ControlComponent::Forward) {
            const auto orien = camera->rotation;
            const auto delta = glm::rotate(orien, glm::vec4{0.f, 0.f, -distance, 0.f});
            camera->translation += glm::vec3{delta};
        }

        if (cameraActions.actions & ControlComponent::Back) {
            const auto orien = camera->rotation;
            const auto delta = glm::rotate(orien, glm::vec4{0.f, 0.f, distance, 0.f});
            camera->translation += glm::vec3{delta};
        }

        if (cameraActions.actions & ControlComponent::StrafeRight) {
            const auto orien = camera->rotation;
            const auto delta = glm::rotate(orien, glm::vec4{distance, 0.f, 0.f, 0.f});
            camera->translation += glm::vec3{delta};
        }

        if (cameraActions.actions & ControlComponent::StrafeLeft) {
            const auto orien = camera->rotation;
            const auto delta = glm::rotate(orien, glm::vec4{-distance, 0.f, 0.f, 0.f});
            camera->translation += glm::vec3{delta};
        }

        if (cameraActions.actions & ControlComponent::Up) {
            const auto orien = camera->rotation;
            const auto delta = glm::rotate(orien, glm::vec4{0.f, distance, 0.f, 0.f});
            camera->translation += glm::vec3{delta};
        }

        if (cameraActions.actions & ControlComponent::Down) {
            const auto orien = camera->rotation;
            const auto delta = glm::rotate(orien, glm::vec4{0.f, -distance, 0.f, 0.f});
            camera->translation += glm::vec3{delta};
        }
    }
};

//------------------------------------------------------------------------------

class TppCameraController : public CameraController
{
  public:
    void execute(float /*deltaTime*/, Actor* /*actor*/) override
    {
        // Fixed position relative to player
        auto orien          = player->rotation;
        const auto delta    = glm::rotate(orien, glm::vec4{0.f, 1.5f, -6.f, 0.f});
        camera->translation    = player->translation + glm::vec3{delta};
        camera->rotation = orien * glm::quat{0.f, 0.f, 1.f, 0.f};
    }
};

#endif /* CAMERACONTROLLER_H */
