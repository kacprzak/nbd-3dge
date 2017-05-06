#include "Engine.h"

#include "Logger.h"
//#include "network/BaseSocketMgr.h"

#include <cstdlib> // exit
#include <sstream>

// Maximum delta value passed to update 1/25 [s]
#define DELTA_MAX 0.04f

Engine::Engine(bool initVideo)
    : m_breakLoop(false)
    , m_initVideo(initVideo)
    , m_appActive(true)
    , m_mouseFocus(true)
    , m_inputFocus(true)
    , m_game(nullptr)
{
    try {
        initializeSDL();
    } catch (const EngineError& /*e*/) {
        SDL_Quit();
        throw;
    }
}

//------------------------------------------------------------------------------

Engine::~Engine()
{
    LOG_INFO << "Quitting SDL";
    SDL_Quit();
}

//------------------------------------------------------------------------------

void Engine::initializeSDL()
{
    LOG_INFO << "Initializing SDL";

    logSDLInfo();

    uint32_t sdl_flags = 0;
    if (m_initVideo) {
        sdl_flags |= SDL_INIT_VIDEO;
    }

    if (SDL_Init(sdl_flags) < 0) {
        throw EngineError("Could not initialize SDL", SDL_GetError());
    }

    if (m_initVideo) {
        LOG_INFO << "Current video driver: " << SDL_GetCurrentVideoDriver();
    }

    LOG_INFO << "SDL initialized";
}

//------------------------------------------------------------------------------

void Engine::logSDLInfo()
{
    SDL_version compiled;
    SDL_version linked;

    SDL_VERSION(&compiled);
    SDL_GetVersion(&linked);

    std::stringstream ss;
    LOG_INFO << "Compiled against SDL: " << (uint32_t)compiled.major << "."
             << (uint32_t)compiled.minor << "." << (uint32_t)compiled.patch;
    LOG_INFO << "Linked against SDL: " << (uint32_t)linked.major << "." << (uint32_t)linked.minor
             << "." << (uint32_t)linked.patch;

    if (m_initVideo) {
        ss << "Video drivers available: ";
        for (int i = 0; i < SDL_GetNumVideoDrivers(); ++i) {
            ss << SDL_GetVideoDriver(i) << ' ';
        }
    }
    LOG_INFO << ss.str();
}

//------------------------------------------------------------------------------

void Engine::mainLoop(GameLogic* game)
{
    m_game = game;
    m_game->onBeforeMainLoop(this);

    unsigned int curr_time = SDL_GetTicks();
    unsigned int last_time = curr_time;
    float delta            = 0.0f;

    for (;;) {
        if (m_breakLoop || !processEvents()) break;
        if (m_appActive) {
            if (/*m_inputFocus &&*/ delta > 0.0f) {
                while (delta > DELTA_MAX) {
                    update(DELTA_MAX);
                    delta -= DELTA_MAX;
                }
                update(delta);
            }
            draw();
        }

        curr_time = SDL_GetTicks();
        delta     = float(curr_time - last_time) / 1000.0f;
        last_time = curr_time;
    }

    m_game->onAfterMainLoop(this);
}

//------------------------------------------------------------------------------

/*! Return true if should keep going. */
bool Engine::processEvents()
{
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_KEYUP: break;
        case SDL_KEYDOWN:
            if (event.key.keysym.scancode == SDL_SCANCODE_G) m_game->toggleDrawDebug();
            if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) return false;
            break;
        case SDL_WINDOWEVENT: {
            switch (event.window.event) {
            case SDL_WINDOWEVENT_FOCUS_GAINED: m_inputFocus = true; break;
            case SDL_WINDOWEVENT_FOCUS_LOST: m_inputFocus   = false; break;
            default: break;
            }
            break;
        }
        case SDL_QUIT: return false;
        }

        for (auto& gv : m_game->gameViews()) {
            gv->processInput(event);
        }
    }

    return true;
}

//------------------------------------------------------------------------------

void Engine::update(float elapsedTime)
{
    // Update game logic
    m_game->update(elapsedTime);

    // Update views
    for (auto gv : m_game->gameViews())
        gv->update(elapsedTime);

    // Update network
    // net::BaseSocketMgr* sm = net::BaseSocketMgr::singletonPtr();
    // if (sm)
    //    sm->select(20);
}

//------------------------------------------------------------------------------

void Engine::draw()
{
    if (!m_initVideo) return;

    m_game->draw();

    // Draw game
    for (auto gv : m_game->gameViews()) {
        gv->draw();
    }
}

//------------------------------------------------------------------------------

int Engine::showErrorMessageBox(const char* msg)
{
    return SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", msg, NULL);
}
