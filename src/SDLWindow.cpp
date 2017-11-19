#include "SDLWindow.h"

#include "Engine.h"
#include "FpsCounter.h"
#include "Logger.h"

#ifdef _WIN32
#include <windows.h>
#else
#define APIENTRY
#endif

#ifndef NDEBUG
static void APIENTRY openglCallbackFunction(GLenum /*source*/, GLenum /*type*/, GLuint /*id*/,
                                            GLenum severity, GLsizei /*length*/,
                                            const GLchar* message, const void* /*userParam*/)
{
    if (severity != GL_DEBUG_SEVERITY_NOTIFICATION) {
        if (severity == GL_DEBUG_SEVERITY_HIGH) {
            LOG_ERROR << message;
            abort();
        } else {
            LOG_WARNING << message;
        }
    }
}
#endif

//==============================================================================

SDLWindow::SDLWindow(const Settings& settings)
    : m_title{settings.windowTitle}
    , m_screenWidth{settings.screenWidth}
    , m_screenHeight{settings.screenHeight}
    , m_screenFull{settings.fullscreen}
    , m_msaa{settings.msaa}
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
    int screen_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;

    if (m_screenFull) screen_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;

    if (m_msaa != 0) {
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, m_msaa);
    }

    // Screen surface
    m_window = SDL_CreateWindow(m_title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                m_screenWidth, m_screenHeight, screen_flags);

    if (!m_window) {
        throw EngineError("Creating window failed", SDL_GetError());
    }

    // SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
    // SDL_StopTextInput(); // Disable text input events when GUI is not visible

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
#ifndef NDEBUG
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1);

    m_glContext = SDL_GL_CreateContext(m_window);

    if (!m_glContext) {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        // sRGB on mesa driver is not working without this line
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
        m_glContext = SDL_GL_CreateContext(m_window);
    }

    if (!m_glContext) {
        SDL_DestroyWindow(m_window);
        throw EngineError("Creating OpenGL context failed", SDL_GetError());
    }

    int contexMajorVersion;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &contexMajorVersion);
    int contexMinorVersion;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &contexMinorVersion);
    LOG_INFO << "  GLContextVersion: " << contexMajorVersion << '.' << contexMinorVersion;

    int redSize;
    SDL_GL_GetAttribute(SDL_GL_RED_SIZE, &redSize);
    int greenSize;
    SDL_GL_GetAttribute(SDL_GL_GREEN_SIZE, &greenSize);
    int blueSize;
    SDL_GL_GetAttribute(SDL_GL_BLUE_SIZE, &blueSize);
    int alphaSize;
    SDL_GL_GetAttribute(SDL_GL_ALPHA_SIZE, &alphaSize);
    LOG_INFO << "  RGBA sizes: " << redSize << " " << greenSize << " " << blueSize << " "
             << alphaSize;

    int depthSize;
    SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &depthSize);
    LOG_INFO << "  Depth size: " << depthSize;

    int multisampleBuffers;
    SDL_GL_GetAttribute(SDL_GL_MULTISAMPLEBUFFERS, &multisampleBuffers);
    int multisampleSamples;
    SDL_GL_GetAttribute(SDL_GL_MULTISAMPLESAMPLES, &multisampleSamples);
    LOG_INFO << "  Multisample buffers: " << multisampleBuffers
             << " samples: " << multisampleSamples;

    toggleVSync();

    // No SDL related stuff
    initializeOpenGL(contexMajorVersion, contexMinorVersion);
}

void SDLWindow::toggleVSync()
{
    m_swapInterval = (m_swapInterval == 0) ? 1 : 0;

    SDL_GL_SetSwapInterval(m_swapInterval);
    m_swapInterval = SDL_GL_GetSwapInterval();
    LOG_INFO << "SwapInterval: " << m_swapInterval;
}

SDLWindow::~SDLWindow()
{
    SDL_GL_DeleteContext(m_glContext);
    SDL_DestroyWindow(m_window);
}

bool SDLWindow::processInput(const SDL_Event& event)
{
    switch (event.type) {
    case SDL_KEYUP: keyReleased(event); break;
    case SDL_KEYDOWN: keyPressed(event); break;
    case SDL_MOUSEMOTION: mouseMoved(event); break;
    case SDL_MOUSEBUTTONUP: mouseButtonReleased(event); break;
    case SDL_MOUSEBUTTONDOWN: mouseButtonPressed(event); break;
    case SDL_WINDOWEVENT: {
        switch (event.window.event) {
        case SDL_WINDOWEVENT_SIZE_CHANGED:
            resizeWindow(event.window.data1, event.window.data2);
            break;
        }
        break;
    }
    }

    return true;
}

void SDLWindow::initializeOpenGL(int contextMajorVersion, int contextMinorVersion)
{
    glewExperimental      = GL_TRUE;
    GLenum glewInitStatus = glewInit();

    if (GLEW_OK != glewInitStatus) {
        LOG_FATAL << "glewInitStatus: " << glewGetErrorString(glewInitStatus);
        exit(1);
    }

    if (m_msaa != 0) glEnable(GL_MULTISAMPLE);
    LOG_GL_ERROR;

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    /* Depth buffer setup */
    glClearDepth(1.0f);

    /* Enables Depth Testing */
    glEnable(GL_DEPTH_TEST);
    LOG_GL_ERROR;

    /* The Type Of Depth Test To Do */
    glDepthFunc(GL_LEQUAL);

    /* Render only front of polygon */
    glEnable(GL_CULL_FACE);
    LOG_GL_ERROR;

    /* Gamma correction */
    glEnable(GL_FRAMEBUFFER_SRGB);
    LOG_GL_ERROR;

#ifndef NDEBUG
    if (contextMajorVersion >= 4 && contextMinorVersion >= 3) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(openglCallbackFunction, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, true);
    }
#endif
}

//------------------------------------------------------------------------------

void SDLWindow::resizeWindow(int width, int height)
{
    /* Setup our viewport. */
    glViewport(0, 0, GLsizei(width), GLsizei(height));
}

//------------------------------------------------------------------------------

void SDLWindow::preDraw() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }

//------------------------------------------------------------------------------

void SDLWindow::postDraw() { SDL_GL_SwapWindow(m_window); }

//------------------------------------------------------------------------------
