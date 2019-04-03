#ifndef SDLWINDOW_H
#define SDLWINDOW_H

#include "GameView.h"

#include "Settings.h"
#include <GL/glew.h>

class SDLWindow : public GameView
{
  public:
    SDLWindow(const Settings& settings);
    ~SDLWindow();

    bool processInput(const SDL_Event& event) override final;

  protected:
    virtual void resizeWindow(int width, int height);

    virtual void mouseMoved(const SDL_Event& event)          = 0;
    virtual void mouseButtonPressed(const SDL_Event& event)  = 0;
    virtual void mouseButtonReleased(const SDL_Event& event) = 0;

    virtual void keyPressed(const SDL_Event& event)  = 0;
    virtual void keyReleased(const SDL_Event& event) = 0;

    void preDraw();
    void postDraw();

    void toggleVSync();

  private:
    void createSDLWindow();
    void initializeOpenGL(int contextMajorVersion, int contextMinorVersion);
    void initializeGui();

    std::string m_title;
    int m_screenWidth;
    int m_screenHeight;
    bool m_screenFull;
    int m_msaa;

    SDL_Window* m_window;
    SDL_GLContext m_glContext;
    int m_swapInterval = 0; //< Vsync on:1 off:0
};

#endif // SDLWINDOW_H
