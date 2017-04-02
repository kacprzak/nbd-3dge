#ifndef GAMECORE_H
#define GAMECORE_H

#include <GL/glew.h>
#include <SFML/Window.hpp>
//#include <SFML/OpenGL.hpp>
#include "GameObjectManager.h"

class GameCore
{
public:
    GameCore();
    virtual ~GameCore();

    void mainLoop();

protected:
    virtual void resizeWindow(int width, int height);

    virtual void draw();
    virtual void update(float delta);
    virtual void mouseWheelMoved(int wheelDelta) = 0;

    GameObjectManager& gameObjectManager() { return m_gom; }
    sf::Window& getWindow() { return *m_window; }

private:
    void initGL();

    GameObjectManager m_gom;
    sf::Window *m_window;
};

#endif // GAMECORE_H
