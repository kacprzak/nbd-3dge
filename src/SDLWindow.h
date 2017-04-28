#ifndef GAMECORE_H
#define GAMECORE_H

#include "GameView.h"

#include "GameObjectManager.h"
#include "Settings.h"

#include <GL/glew.h>

class SDLWindow : public GameView
{
public:
    SDLWindow(const Settings& settings);
    virtual ~SDLWindow();

    void update(float delta) override;

protected:
    bool processInput(const SDL_Event& event) override;
    
    virtual void resizeWindow(int width, int height);
    
    virtual void mouseMoved(const SDL_Event& event) = 0;
    virtual void keyPressed(const SDL_Event& event) = 0;
    virtual void keyReleased(const SDL_Event& event) = 0;

    GameObjectManager& gameObjectManager() { return m_gom; }

    void preDraw();
    void postDraw();

 private:
    void createSDLWindow();

    void initializeOpenGL();

    std::string m_title;
    int m_screenWidth;
    int m_screenHeight;
    bool m_screenFull;

    SDL_Window* m_window;
    SDL_GLContext m_glContext;

    GameObjectManager m_gom;
};

#endif // GAMECORE_H
