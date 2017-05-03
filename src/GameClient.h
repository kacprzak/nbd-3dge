#ifndef GAMECLIENT_H
#define GAMECLIENT_H

#include "SDLWindow.h"
#include "ResourcesMgr.h"
#include "Camera.h"
#include "Settings.h"
#include "GfxScene.h"
#include "FpsCounter.h"
#include "Components.h"

class GameClient : public SDLWindow
{
    typedef SDLWindow super;
  
 public:
    GameClient(const Settings& settings);

    void loadResources(const std::string& xmlFile);
    void unloadResources() override;

    void addActor(int id, TransformationComponent* tr, RenderComponent* rd) override;
    void removeActor(int id) override;

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
    void loadData(const Settings& s);

    std::unique_ptr<ResourcesMgr> m_resourcesMgr;
    std::shared_ptr<Camera> m_camera;
    std::shared_ptr<ShaderProgram> m_normalsShader;

    const Settings m_settings;
    GfxScene m_scene;
    FpsCounter m_fpsCounter;

    float m_cameraSpeed = 50.0f;

    bool m_wPressed = false;
    bool m_sPressed = false;
    bool m_aPressed = false;
    bool m_dPressed = false;
    bool m_shiftPressed = false;
    bool m_leftMouseButtonPressed = false;
};

#endif // GAMECLIENT_H
