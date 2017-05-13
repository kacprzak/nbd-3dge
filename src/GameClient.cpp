#include "GameClient.h"

#include "Shader.h"
#include "Skybox.h"
#include "Terrain.h"
#include "Texture.h"

//#define GLM_FORCE_RADIANS

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

GameClient::GameClient(const Settings& settings, std::shared_ptr<ResourcesMgr> resourcesMgr)
    : SDLWindow{settings}
    , m_resourcesMgr{resourcesMgr}
    , m_settings{settings}
{
    if (!m_resourcesMgr) {
        m_resourcesMgr =
            std::make_shared<ResourcesMgr>(m_settings.dataFolder, m_settings.shadersFolder);
    }
}

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
    if (ctrl) m_inputSystem.addActor(id, ctrl);
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
    float cameraSpeedMultiplyer               = 0.5f;
    if (m_shiftPressed) cameraSpeedMultiplyer = 5.0f;

    auto distance  = delta * m_cameraSpeed * cameraSpeedMultiplyer;
    Camera* camera = m_renderSystem.getCamera();

    if (m_wPressed) {
        const auto orien = camera->transformation()->orientation;
        const auto delta = glm::rotate(orien, glm::vec4{0.f, 0.f, -distance, 0.f});
        camera->transformation()->position += glm::vec3{delta};
    }

    if (m_sPressed) {
        const auto orien = camera->transformation()->orientation;
        const auto delta = glm::rotate(orien, glm::vec4{0.f, 0.f, distance, 0.f});
        camera->transformation()->position += glm::vec3{delta};
    }

    if (m_dPressed) {
        const auto orien = camera->transformation()->orientation;
        const auto delta = glm::rotate(orien, glm::vec4{distance, 0.f, 0.f, 0.f});
        camera->transformation()->position += glm::vec3{delta};
    }

    if (m_aPressed) {
        const auto orien = camera->transformation()->orientation;
        const auto delta = glm::rotate(orien, glm::vec4{-distance, 0.f, 0.f, 0.f});
        camera->transformation()->position += glm::vec3{delta};
    }

    m_renderSystem.update(delta);
}

//------------------------------------------------------------------------------

void GameClient::mouseMoved(const SDL_Event& event)
{
    float mouseSensity = 0.2f;

    if (m_leftMouseButtonPressed) {
        m_renderSystem.getCamera()->rotate(event.motion.xrel * mouseSensity,
                                           event.motion.yrel * mouseSensity, 0.0f);
    }
    m_inputSystem.mouseMoved(event);
}

void GameClient::mouseButtonPressed(const SDL_Event& event)
{
    switch (event.button.button) {
    case SDL_BUTTON_LEFT:
        m_leftMouseButtonPressed = true;
        setMouseRelativeMode(true);
        break;
    }
    m_inputSystem.mouseButtonPressed(event);
}

void GameClient::mouseButtonReleased(const SDL_Event& event)
{
    switch (event.button.button) {
    case SDL_BUTTON_LEFT:
        m_leftMouseButtonPressed = false;
        setMouseRelativeMode(false);
        break;
    }
    m_inputSystem.mouseButtonReleased(event);
}

//------------------------------------------------------------------------------

void GameClient::keyPressed(const SDL_Event& event)
{
    switch (event.key.keysym.sym) {
    case SDLK_w: m_wPressed = true; break;
    case SDLK_s: m_sPressed = true; break;
    case SDLK_a: m_aPressed = true; break;
    case SDLK_d: m_dPressed = true; break;
    }

    switch (event.key.keysym.scancode) {
    case SDL_SCANCODE_LSHIFT: m_shiftPressed = true; break;
    default: break;
    }
    m_inputSystem.keyPressed(event);
}

void GameClient::keyReleased(const SDL_Event& event)
{
    switch (event.key.keysym.sym) {
    case SDLK_w: m_wPressed = false; break;
    case SDLK_s: m_sPressed = false; break;
    case SDLK_a: m_aPressed = false; break;
    case SDLK_d: m_dPressed = false; break;
    }

    switch (event.key.keysym.scancode) {
    case SDL_SCANCODE_LSHIFT: m_shiftPressed = false; break;
    case SDL_SCANCODE_Z: m_renderSystem.setNextPolygonMode(); break;
    case SDL_SCANCODE_N: {
        static int magnitude = 2;

        if (event.key.keysym.mod & KMOD_SHIFT) {
            magnitude = std::abs(++magnitude);
            m_renderSystem.setDrawNormals(m_renderSystem.isDrawNormals(), magnitude);
        } else if (event.key.keysym.mod & KMOD_CTRL) {
            magnitude                     = std::abs(--magnitude);
            if (magnitude == 0) magnitude = 1;
            m_renderSystem.setDrawNormals(m_renderSystem.isDrawNormals(), magnitude);
        } else {
            m_renderSystem.setDrawNormals(!m_renderSystem.isDrawNormals(), magnitude);
        }
    } break;
    case SDL_SCANCODE_V: toggleVSync(); break;
    default: break;
    }
    m_inputSystem.keyReleased(event);
}
