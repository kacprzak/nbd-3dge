#ifndef GAMECORE_H
#define GAMECORE_H

#include <GL/glew.h>
#include <SFML/Window.hpp>
#include "GameObjectManager.h"

class GameCore
{
public:
    GameCore();
    virtual ~GameCore();

    void mainLoop();

protected:
    virtual void resizeWindow(int width, int height);

    virtual void draw() = 0;
    virtual void update(float delta);
    
    virtual void mouseWheelMoved(int wheelDelta) = 0;
    virtual void keyPressed(const sf::Event::KeyEvent& e) = 0;
    virtual void keyReleased(const sf::Event::KeyEvent& e) = 0;

    GameObjectManager& gameObjectManager() { return m_gom; }
    //sf::Window& getWindow() { return *m_window; }
    sf::Vector2i getMousePosition() const;
    void toggleVSync();

private:
    void initGL();

    GameObjectManager m_gom;
    sf::Window *m_window;

    bool m_vSyncEnabled = false;
};

#endif // GAMECORE_H
