#ifndef GAME_H
#define GAME_H

#include <GL/glew.h>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>

#include "gameobjectmanager.h"

class Game
{
public:
    Game();
    virtual ~Game();

    void mainLoop();

protected:
    GameObjectManager m_gom;

private:
    void initGL();
    bool resizeWindow(int width, int height);
    void draw();
    void update(float delta);
    void mouseWheelMoved(int wheelDelta);
    void polarView();

    sf::Window *m_window;

    float m_zoom;
    float m_elevation;
    float m_azimuth;

};

#endif // GAME_H
