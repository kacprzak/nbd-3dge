#include "GameClient.h"

#include "Camera.h"
#include "CameraController.h"
#include "Scene.h"
#include "Shader.h"
#include "Skybox.h"
#include "Terrain.h"
#include "Texture.h"

GameClient::GameClient(const Settings& settings, const std::shared_ptr<ResourcesMgr>& resourcesMgr)
    : SDLWindow{settings}
    , m_resourcesMgr{resourcesMgr}
    , m_settings{settings}
    , m_renderSystem({m_settings.screenWidth, m_settings.screenHeight})
{
    if (!m_resourcesMgr) {
        m_resourcesMgr =
            std::make_shared<ResourcesMgr>(m_settings.dataFolder, m_settings.shadersFolder);
    }

    m_freeCameraCtrl = std::make_unique<FreeCameraController>();
    // m_freeCameraCtrl->camera = m_renderSystem.getCamera(RenderSystem::Free)->worldTranslation();
    m_inputSystem.addActor(-1, &m_freeCameraCtrl->cameraActions);

    m_tppCameraCtrl = std::make_unique<TppCameraController>();
    // m_tppCameraCtrl->camera = m_renderSystem.getCamera(RenderSystem::Player)->worldTranslation();
}

//------------------------------------------------------------------------------

GameClient::~GameClient() { m_inputSystem.removeActor(-1); }

//------------------------------------------------------------------------------

void GameClient::resizeWindow(int width, int height)
{
    /* Protect against a divide by zero */
    if (height == 0) height = 1;

    m_renderSystem.resizeWindow({width, height});
    super::resizeWindow(width, height);
}

//------------------------------------------------------------------------------

void GameClient::loadResources(const std::string& file)
{
    m_resourcesMgr->load(file);
    m_renderSystem.loadCommonResources(*m_resourcesMgr);
    // Scene scene;
    m_scene = std::make_shared<Scene>();
    // m_scene->load(m_settings.dataFolder + "untitled.gltf");
    // m_scene->load(m_settings.dataFolder +
    //              "glTF-Sample-Models-master/2.0/BoomBox/glTF/BoomBox.gltf");
    m_scene->load(m_settings.dataFolder + "glTF-Sample-Models-master/2.0/Corset/glTF/Corset.gltf");

    m_renderSystem.setScene(m_scene);

    m_resourcesFile = file;

    auto* cam = m_renderSystem.findNode("Camera");
    if (cam) {
        static TransformationComponent tr;
        tr.translation = cam->getTranslation();
        tr.rotation    = cam->getRotation();

        m_freeCameraCtrl->camera = &tr;
    }
    // m_tppCameraCtrl->camera = &tr;
}

//------------------------------------------------------------------------------

void GameClient::unloadResources() { m_resourcesMgr.reset(); }

//------------------------------------------------------------------------------

void GameClient::addActor(int id, TransformationComponent* tr, RenderComponent* rd,
                          LightComponent* lt, ControlComponent* ctrl)
{
    m_renderSystem.addActor(id, tr, rd, lt, *m_resourcesMgr);
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
    // m_tppCameraCtrl->execute(delta, nullptr);
    if (m_inputSystem.isMouseRelativeMode()) {
        m_freeCameraCtrl->execute(delta, nullptr);

        auto* cam = m_renderSystem.findNode("Camera");
        cam->setTranslation(m_freeCameraCtrl->camera->translation);
        cam->setRotation(m_freeCameraCtrl->camera->rotation);
    }
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
            ++magnitude;
            magnitude = std::abs(magnitude);
            m_renderSystem.setDrawNormals(m_renderSystem.isDrawNormals(), magnitude);
        } else if (event.key.keysym.mod & KMOD_CTRL) {
            --magnitude;
            magnitude = std::abs(magnitude);

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
            m_renderSystem.setCamera(RenderSystem::Free);
        } else {
            m_renderSystem.setCamera(RenderSystem::Player);
        }
        m_inputSystem.setDebug(debugCamera);
    } break;
    case SDL_SCANCODE_L: {
        m_resourcesMgr->loadShaders(m_resourcesFile);
        // m_resourcesMgr->loadMaterials(m_resourcesFile);
    } break;
    default: break;
    }
    m_inputSystem.keyReleased(event);
}
