#include "SDLWindow.h"

#include "Engine.h"
#include "FpsCounter.h"

#include <iostream>

#ifdef _WIN32
#include <windows.h>
#else
#define APIENTRY
#endif

static void APIENTRY openglCallbackFunction(GLenum /*source*/,
                                            GLenum /*type*/,
                                            GLuint /*id*/,
                                            GLenum severity,
                                            GLsizei /*length*/,
                                            const GLchar* message,
                                            const void* /*userParam*/)
{
    if (severity != GL_DEBUG_SEVERITY_NOTIFICATION) {
        fprintf(stderr, "%s\n", message);
        if (severity == GL_DEBUG_SEVERITY_HIGH) {
            fprintf(stderr, "Aborting...\n");
            abort();
        }
    }
}

SDLWindow::SDLWindow(const Settings& settings)
    : m_title{settings.windowTitle},
      m_screenWidth{settings.screenWidth},
      m_screenHeight{settings.screenHeight},
      m_screenFull{settings.fullscreen}
{
    try {
        createSDLWindow();
    } catch (EngineError /*e*/) {
        SDL_Quit();
        throw;
    }
}

void SDLWindow::createSDLWindow()
{
    int screen_flags = SDL_WINDOW_OPENGL;

    if (m_screenFull)
        screen_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;

    // Screen surface
    m_window = SDL_CreateWindow(m_title.c_str(), SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED, m_screenWidth,
                                m_screenHeight, screen_flags);

    if (!m_window) {
        throw EngineError("Creating window failed", SDL_GetError());
    }

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE,   8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,  8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
#ifndef NDEBUG
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetSwapInterval(1);

    m_glContext = SDL_GL_CreateContext(m_window);
    if (!m_glContext) {
        SDL_DestroyWindow(m_window);
        throw EngineError("Creating OpenGL context failed", SDL_GetError());
    }

    // No SDL related stuff
    initializeOpenGL();
}

SDLWindow::~SDLWindow()
{
    SDL_GL_DeleteContext(m_glContext);
    SDL_DestroyWindow(m_window);
}

bool SDLWindow::processInput(const SDL_Event& event)
{
    switch (event.type) {
    case SDL_KEYUP:
        keyReleased(event);
        break;
    case SDL_KEYDOWN:
        keyPressed(event);
        break;
    case SDL_MOUSEMOTION:
        mouseMoved(event);
        break;
    }

    return true;
}

void SDLWindow::update(float delta)
{
    m_gom.update(delta);
}

void SDLWindow::initializeOpenGL()
{
    GLenum glewInitStatus = glewInit();

    if (GLEW_OK != glewInitStatus) {
        std::cerr << "ERROR: " << glewGetErrorString(glewInitStatus);
        exit(1);
    }

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    /* Depth buffer setup */
    glClearDepth(1.0f);

    /* Enables Depth Testing */
    glEnable(GL_DEPTH_TEST);

    /* The Type Of Depth Test To Do */
    glDepthFunc(GL_LESS);

    /* Render only front of polygon */
    glEnable(GL_CULL_FACE);

    /* Gamma correction */
    glEnable(GL_FRAMEBUFFER_SRGB);

#ifndef NDEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(openglCallbackFunction, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE,
                          0, NULL, true);
#endif
}

/* function to reset our viewport after a window resize */
void SDLWindow::resizeWindow(int width, int height)
{
    //std::cout << "resizeWindow " << width << " " << height << std::endl;

    /* Setup our viewport. */
    glViewport(0, 0, GLsizei(width), GLsizei(height));
}

//------------------------------------------------------------------------------

void SDLWindow::preDraw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

//------------------------------------------------------------------------------

void SDLWindow::postDraw()
{
    SDL_GL_SwapWindow(m_window);
}
