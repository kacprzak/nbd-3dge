#ifndef GAMECORE_H
#define GAMECORE_H

#include <GL/glew.h>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>

#include "gameobjectmanager.h"
#include <glm/glm.hpp>

class GameCore
{
public:
    GameCore();
    virtual ~GameCore();

    void mainLoop();

protected:
    GameObjectManager m_gom;
    glm::mat4 m_projectionMatrix;

    virtual void draw();
    virtual void update(float delta);
    virtual void mouseWheelMoved(int wheelDelta) = 0;

private:
    void initGL();
    bool resizeWindow(int width, int height);

    sf::Window *m_window;
};

#endif // GAMECORE_H
