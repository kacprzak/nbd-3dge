#ifndef GAME_H
#define GAME_H

#include "GameCore.h"
#include "ResourcesMgr.h"
#include "Camera.h"

class Game : public GameCore
{
    typedef GameCore super;
  
 public:
    Game();

 protected:
    void resizeWindow(int width, int height) override;

    void draw() override;
    void update(float delta) override;
    void mouseWheelMoved(int wheelDelta) override;
    void keyPressed(const sf::Event::KeyEvent& e) override;
    void keyReleased(const sf::Event::KeyEvent& e) override;

 private:
    void init();
    void loadData();
    void polarView();

    std::unique_ptr<ResourcesMgr> m_resourcesMgr;
    Camera *m_camera;
};

#endif // GAME_H
