#ifndef GAME_H
#define GAME_H

#include "GameCore.h"
#include "ResourcesMgr.h"
#include "Camera.h"

class Game : public GameCore
{
    typedef GameCore super;
  
 public:
    Game(const std::string& title, int screenWidth,
         int screenHeight, bool screenFull);

 protected:
    void resizeWindow(int width, int height) override;

    void draw() override;
    void update(float delta) override;

    void mouseMoved(const SDL_Event& event) override;
    void keyPressed(const SDL_Event& event) override;
    void keyReleased(const SDL_Event& event) override;

 private:
    void loadData();
    void polarView();

    std::unique_ptr<ResourcesMgr> m_resourcesMgr;
    std::shared_ptr<Camera> m_camera;
    std::shared_ptr<ShaderProgram> m_normalsShader;

    float m_cameraSpeed = 50.0f;

    bool m_wPressed = false;
    bool m_sPressed = false;
    bool m_aPressed = false;
    bool m_dPressed = false;
};

#endif // GAME_H
