#include "GameClient.h"

#include "CameraController.h"
#include "Terrain.h"
#include "gfx/Camera.h"
#include "gfx/Model.h"
#include "gfx/Shader.h"
#include "gfx/Skybox.h"
#include "gfx/Texture.h"

#include "loaders/GltfLoader.h"

#include <imgui.h>

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

    static TransformationComponent tr;
    m_freeCameraCtrl->camera = &tr;
    m_inputSystem.addActor(-1, &m_freeCameraCtrl->cameraActions);

    // m_tppCameraCtrl = std::make_unique<TppCameraController>();
    // m_tppCameraCtrl->camera = m_renderSystem.getCamera(RenderSystem::Player)->worldTranslation();
    m_renderSystem.setCamera(&m_camera);
    m_renderSystem.resizeWindow({m_settings.screenWidth, m_settings.screenHeight});
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

    m_resourcesFile = file;

    // auto* cam = m_renderSystem.findNode("Camera");
    // if (cam) {
    //   static TransformationComponent tr;
    //  tr.translation = cam->getTranslation();
    // tr.rotation    = cam->getRotation();

    //        m_freeCameraCtrl->camera = &tr;
    //   }
    // m_tppCameraCtrl->camera = &tr;
}

//------------------------------------------------------------------------------

void GameClient::unloadResources() { m_resourcesMgr.reset(); }

//------------------------------------------------------------------------------

void GameClient::addActor(int id, TransformationComponent* tr, RenderComponent* rd,
                          LightComponent* lt, ControlComponent* ctrl)
{
    if (rd) {
        auto model = m_renderSystem.findModel(rd->model);
        if (!model) {
            loaders::GltfLoader loader{m_settings.dataFolder};
            loader.load(rd->model);
            model = loader.model();
            m_renderSystem.addModel(model);
        }
    }

    m_renderSystem.addActor(id, tr, rd, lt, *m_resourcesMgr);
    if (ctrl) {
        m_inputSystem.addActor(id, ctrl);
        // m_tppCameraCtrl->player = tr;
    }

    m_freeCameraCtrl->camera->translation = m_camera.worldTranslation();
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

    static bool showNormals    = false;
    static float normalLength  = 1.0f;
    static float tangentLength = 1.0f;
    static glm::vec4 ntbLenghts{1.0f, 1.0f, 1.0f, 1.0f};
    static bool vsync = true;

    ImGui::Begin("Debug");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                ImGui::GetIO().Framerate);

    if (ImGui::Checkbox("VSync", &vsync)) {
        toggleVSync();
    }
    if (ImGui::Button("Polygon Mode")) {
        m_renderSystem.setNextPolygonMode();
    }
    if (ImGui::Checkbox("Draw debug", &showNormals)) {
        m_renderSystem.setDrawDebug(!m_renderSystem.isDrawDebug(), ntbLenghts);
    }
    ImGui::Text("Vertex");
    if (ImGui::SliderFloat("Normal ", &ntbLenghts[3], 0.0f, 10.0f)) {
        m_renderSystem.setDrawDebug(m_renderSystem.isDrawDebug(), ntbLenghts);
    }
    ImGui::Text("Face");
    if (ImGui::SliderFloat("Normal", &ntbLenghts[0], 0.0f, 10.0f)) {
        m_renderSystem.setDrawDebug(m_renderSystem.isDrawDebug(), ntbLenghts);
    }
    if (ImGui::SliderFloat("Tangent", &ntbLenghts[1], 0.0f, 10.0f)) {
        m_renderSystem.setDrawDebug(m_renderSystem.isDrawDebug(), ntbLenghts);
    }
    if (ImGui::SliderFloat("Bitangent", &ntbLenghts[2], 0.0f, 10.0f)) {
        m_renderSystem.setDrawDebug(m_renderSystem.isDrawDebug(), ntbLenghts);
    }
    ImGui::End();

    postDraw();
}

//------------------------------------------------------------------------------

void GameClient::update(float delta)
{
    m_inputSystem.update(delta);
    // m_tppCameraCtrl->execute(delta, nullptr);
    if (m_inputSystem.isMouseRelativeMode()) {
        m_freeCameraCtrl->execute(delta, nullptr);

        // auto* cam = m_renderSystem.findNode("Camera");
        // m_camera.setTranslation(m_freeCameraCtrl->camera->translation);
        // m_camera.setRotation(m_freeCameraCtrl->camera->rotation);
        m_camera.update(m_freeCameraCtrl->camera->toMatrix(), delta);
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
    case SDL_SCANCODE_R:
        m_inputSystem.setMouseRelativeMode(!m_inputSystem.isMouseRelativeMode());
        break;
    case SDL_SCANCODE_TAB: {
        static bool debugCamera = false;
        debugCamera             = !debugCamera;
        /*
                if (debugCamera) {
            m_renderSystem.setCamera(gfx::RenderSystem::Free);
        } else {
            m_renderSystem.setCamera(gfx::RenderSystem::Player);
        }
                */
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
