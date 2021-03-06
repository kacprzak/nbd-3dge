#ifndef ENGINE_H
#define ENGINE_H

#include "GameLogic.h"
#include "config.h"

#if PLATFORM == PLATFORM_WINDOWS
#include <windows.h>
#endif
#include <SDL.h>

#include <stdexcept>
#include <string>

class EngineError : public std::runtime_error
{
  public:
    EngineError(const std::string& msg, const char* sdlError)
        : std::runtime_error(msg + ": " + sdlError)
        , m_sdlError(sdlError)
    {
    }

    const char* sdlError() { return m_sdlError; }

  private:
    const char* m_sdlError;
};

//------------------------------------------------------------------------------

/*!
 * \brief Main loop.
 */
class Engine final
{
  public:
    explicit Engine(bool initVideo = true);
    ~Engine();

    void mainLoop(GameLogic* game);
    GameLogic* game() { return m_game; }

    void breakLoop() { m_breakLoop = true; }

    static int showErrorMessageBox(const char* msg);

  private:
    void initializeSDL();
    void logSDLInfo();

    bool processEvents();
    void update(float elapsedTime);
    void draw();

    bool m_breakLoop = false;
    bool m_appActive = true;
    // bool m_mouseFocus true;
    bool m_inputFocus = true;
    bool m_pause      = false;
    bool m_initVideo;

    GameLogic* m_game = nullptr;
};

#endif
