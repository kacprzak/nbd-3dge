#ifndef GAME_H
#define GAME_H

#include "gamecore.h"
#include "shaderprogram.h"
#include "camera.h"

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

    ShaderProgram *m_sp;
    Camera *m_camera;
};

#endif // GAME_H
