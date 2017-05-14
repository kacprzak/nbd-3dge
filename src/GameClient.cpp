#include "GameClient.h"

#include "Shader.h"
#include "Skybox.h"
#include "Terrain.h"
#include "Texture.h"

//#define GLM_FORCE_RADIANS

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

class CameraController
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
    ControlComponent cameraActions;
    TransformationComponent* camera;
    TransformationComponent* player;

    bool freeCamera     = false; // true;
    float m_cameraSpeed = 50.0f;

    void update(float delta)
    {
        if (freeCamera) {
            float cameraSpeedMultiplyer = 0.5f;

            if (cameraActions.actions & ControlComponent::Fire) cameraSpeedMultiplyer = 5.0f;

            auto distance = delta * m_cameraSpeed * cameraSpeedMultiplyer;

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

        } else {
            // Fixed position relative to player
            auto orien          = player->orientation;
            const auto delta    = glm::rotate(orien, glm::vec4{0.f, 1.5f, -6.f, 0.f});
            camera->position    = player->position + glm::vec3{delta};
            camera->orientation = orien * glm::quat{0.f, 0.f, 1.f, 0.f};
        }
    }
};

//==============================================================================

GameClient::GameClient(const Settings& settings, std::shared_ptr<ResourcesMgr> resourcesMgr)
    : SDLWindow{settings}
    , m_resourcesMgr{resourcesMgr}
    , m_settings{settings}
{
    if (!m_resourcesMgr) {
        m_resourcesMgr =
            std::make_shared<ResourcesMgr>(m_settings.dataFolder, m_settings.shadersFolder);
    }

    m_cameraCtrl         = std::make_unique<CameraController>();
    m_cameraCtrl->camera = m_renderSystem.getCamera()->transformation();
    m_inputSystem.addActor(-1, &m_cameraCtrl.get()->cameraActions);
}

//------------------------------------------------------------------------------

GameClient::~GameClient() {}

//------------------------------------------------------------------------------

void GameClient::resizeWindow(int width, int height)
{
    /* Protect against a divide by zero */
    if (height == 0) height = 1;

    // GLfloat ratio = GLfloat(width) / GLfloat(height);
    // m_renderSystem.getCamera()->setPerspective(45.0f, ratio, 5.0f, 1250.0f);

    super::resizeWindow(width, height);
}

//------------------------------------------------------------------------------

void GameClient::loadResources(const std::string& xmlFile)
{
    m_resourcesMgr->load(xmlFile);
    m_renderSystem.loadCommonResources(*m_resourcesMgr);
}

//------------------------------------------------------------------------------

void GameClient::unloadResources() { m_resourcesMgr.reset(); }

//------------------------------------------------------------------------------

void GameClient::addActor(int id, TransformationComponent* tr, RenderComponent* rd,
                          ControlComponent* ctrl)
{
    m_renderSystem.addActor(id, tr, rd, *m_resourcesMgr);
    if (ctrl) {
        m_inputSystem.addActor(id, ctrl);
        m_cameraCtrl->player = tr;
    }
}

//------------------------------------------------------------------------------

void GameClient::removeActor(int id)
{
    m_inputSystem.removeActor(id);
    m_renderSystem.removeActor(id);
}

//------------------------------------------------------------------------------

void GameClient::draw()
{
    preDraw();

    m_renderSystem.draw();
    m_debugDraw.draw(m_renderSystem.getCamera());

    postDraw();
}

//------------------------------------------------------------------------------

void GameClient::update(float delta)
{
    m_inputSystem.update(delta);
    m_cameraCtrl->update(delta);
    m_renderSystem.update(delta);
}

//------------------------------------------------------------------------------

void GameClient::mouseMoved(const SDL_Event& event) { m_inputSystem.mouseMoved(event); }

void GameClient::mouseButtonPressed(const SDL_Event& event)
{
    m_inputSystem.mouseButtonPressed(event);
}

void GameClient::mouseButtonReleased(const SDL_Event& event)
{
    m_inputSystem.mouseButtonReleased(event);
}

//------------------------------------------------------------------------------

void GameClient::keyPressed(const SDL_Event& event) { m_inputSystem.keyPressed(event); }

void GameClient::keyReleased(const SDL_Event& event)
{
    switch (event.key.keysym.scancode) {
    case SDL_SCANCODE_Z: m_renderSystem.setNextPolygonMode(); break;
    case SDL_SCANCODE_R:
        m_inputSystem.setMouseRelativeMode(!m_inputSystem.isMouseRelativeMode());
        break;
    case SDL_SCANCODE_N: {
        static int magnitude = 2;

        if (event.key.keysym.mod & KMOD_SHIFT) {
            magnitude = std::abs(++magnitude);
            m_renderSystem.setDrawNormals(m_renderSystem.isDrawNormals(), magnitude);
        } else if (event.key.keysym.mod & KMOD_CTRL) {
            magnitude = std::abs(--magnitude);

            if (magnitude == 0) magnitude = 1;
            m_renderSystem.setDrawNormals(m_renderSystem.isDrawNormals(), magnitude);
        } else {
            m_renderSystem.setDrawNormals(!m_renderSystem.isDrawNormals(), magnitude);
        }
    } break;
    case SDL_SCANCODE_V: toggleVSync(); break;
    case SDL_SCANCODE_TAB: m_cameraCtrl->freeCamera = !m_cameraCtrl->freeCamera;
    default: break;
    }
    m_inputSystem.keyReleased(event);
}
