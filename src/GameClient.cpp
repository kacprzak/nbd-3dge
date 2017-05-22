#include "GameClient.h"

#include "CameraController.h"
#include "Shader.h"
#include "Skybox.h"
#include "Terrain.h"
#include "Texture.h"


GameClient::GameClient(const Settings& settings, std::shared_ptr<ResourcesMgr> resourcesMgr)
    : SDLWindow{settings}
    , m_resourcesMgr{resourcesMgr}
    , m_settings{settings}
{
    if (!m_resourcesMgr) {
        m_resourcesMgr =
            std::make_shared<ResourcesMgr>(m_settings.dataFolder, m_settings.shadersFolder);
    }

    m_freeCameraCtrl         = std::make_unique<FreeCamera>();
    m_freeCameraCtrl->camera = m_renderSystem.getCamera()->transformation();

    m_tppCameraCtrl         = std::make_unique<TppCamera>();
    m_tppCameraCtrl->camera = m_renderSystem.getCamera()->transformation();

    m_cameraCtrl = m_tppCameraCtrl.get();
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
        m_tppCameraCtrl->player = tr;
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
    case SDL_SCANCODE_TAB: {
        static bool debugCamera = false;
        debugCamera             = !debugCamera;
        if (debugCamera) {
            m_cameraCtrl = m_freeCameraCtrl.get();
            m_inputSystem.setDebugCamera(&m_cameraCtrl->cameraActions);
        } else {
            m_inputSystem.setDebugCamera(nullptr);
            m_cameraCtrl = m_tppCameraCtrl.get();
        }
    } break;
    default: break;
    }
    m_inputSystem.keyReleased(event);
}
