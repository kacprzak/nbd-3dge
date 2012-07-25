#ifndef GAME_H
#define GAME_H

#include "gamecore.h"
#include "shaderprogram.h"
#include <glm/glm.hpp>

class Game : public GameCore
{
  typedef GameCore super;
  
public:
    Game();

protected:
    void draw();
    void mouseWheelMoved(int wheelDelta);
    void update(float delta);

private:
    void init();
    void loadData();
    void polarView();

    float m_zoom;
    float m_elevation;
    float m_azimuth;

    glm::mat4 m_viewMatrix;
    ShaderProgram *m_sp;
};

#endif // GAME_H
