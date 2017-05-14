#ifndef GAMECLIENT_H
#define GAMECLIENT_H

#include "Components.h"
#include "InputSystem.h"
#include "PhysicsDebugDrawer.h"
#include "RenderSystem.h"
#include "ResourcesMgr.h"
#include "SDLWindow.h"
#include "Settings.h"

class CameraController;

class GameClient : public SDLWindow
{
    typedef SDLWindow super;

  public:
    GameClient(const Settings& settings, std::shared_ptr<ResourcesMgr> resourcesMgr = {});
    ~GameClient();

    void loadResources(const std::string& xmlFile) override;
    void unloadResources() override;

    void addActor(int id, TransformationComponent* tr, RenderComponent* rd,
                  ControlComponent* ctrl) override;
    void removeActor(int id) override;

    PhysicsDebugDrawer* debugDrawer() override { return &m_debugDraw; }

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
    PhysicsDebugDrawer m_debugDraw;
    std::shared_ptr<ResourcesMgr> m_resourcesMgr;

    const Settings m_settings;
    RenderSystem m_renderSystem;
    InputSystem m_inputSystem;

    std::unique_ptr<CameraController> m_cameraCtrl;
};

#endif // GAMECLIENT_H
