#ifndef GAME_H
#define GAME_H

#include "SDLWindow.h"
#include "ResourcesMgr.h"
#include "Camera.h"
#include "Settings.h"
#include "FpsCounter.h"

class Game : public SDLWindow
{
    typedef SDLWindow super;
  
 public:
    Game(const Settings& settings);

 protected:
    void resizeWindow(int width, int height) override;

    void draw() override;
    void update(float delta) override;

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

    FpsCounter m_fpsCounter;

    float m_cameraSpeed = 50.0f;

    bool m_wPressed = false;
    bool m_sPressed = false;
    bool m_aPressed = false;
    bool m_dPressed = false;
    bool m_shiftPressed = false;
    bool m_leftMouseButtonPressed = false;
};

#endif // GAME_H
