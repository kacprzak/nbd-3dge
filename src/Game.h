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
    void resizeWindow(int width, int height);

    void draw();
    void mouseWheelMoved(int wheelDelta);
    void update(float delta);

 private:
    void init();
    void loadData();
    void polarView();

    std::unique_ptr<ResourcesMgr> m_resourcesMgr;
    Camera *m_camera;
};

#endif // GAME_H
