#ifndef GAMECLIENT_H
#define GAMECLIENT_H

#include "Camera.h"
#include "Components.h"
#include "FpsCounter.h"
#include "RenderSystem.h"
#include "PhysicsDebugDrawer.h"
#include "ResourcesMgr.h"
#include "SDLWindow.h"
#include "Settings.h"

class GameClient : public SDLWindow
{
    typedef SDLWindow super;

  public:
    GameClient(const Settings& settings, std::shared_ptr<ResourcesMgr> resourcesMgr = {});

    void loadResources(const std::string& xmlFile) override;
    void unloadResources() override;

    void addActor(int id, TransformationComponent* tr, RenderComponent* rd) override;
    void removeActor(int id) override;

    PhysicsDebugDrawer m_debugDraw;

  protected:
    void resizeWindow(int width, int height) override;

    void update(float delta) override;
    void draw() override;

    void mouseMoved(const SDL_Event& event) override;
    void mouseButtonPressed(const SDL_Event& event) override;
    void mouseButtonReleased(const SDL_Event& event) override;
    void keyPressed(const SDL_Event& event) override;
    void keyReleased(const SDL_Event& event) override;

  private:
    void loadData();

    std::shared_ptr<ResourcesMgr> m_resourcesMgr;
    std::shared_ptr<Camera> m_camera;
    std::shared_ptr<ShaderProgram> m_normalsShader;

    const Settings m_settings;
    RenderSystem m_scene;
    FpsCounter m_fpsCounter;

    float m_cameraSpeed = 50.0f;

    bool m_wPressed               = false;
    bool m_sPressed               = false;
    bool m_aPressed               = false;
    bool m_dPressed               = false;
    bool m_shiftPressed           = false;
    bool m_leftMouseButtonPressed = false;
};

#endif // GAMECLIENT_H
