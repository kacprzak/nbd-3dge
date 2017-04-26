#include "GameCore.h"


#include "Engine.h"

#include <iostream>
#include "FpsCounter.h"

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

GameCore::GameCore(const std::string& title, int screenWidth,
           int screenHeight, bool screenFull)
    : m_title{title}, m_screenWidth{screenWidth}, m_screenHeight{screenHeight}, m_screenFull{screenFull}
{
   try {
        createSDLWindow();
    } catch (EngineError /*e*/) {
        SDL_Quit();
        throw;
   }
}

void GameCore::createSDLWindow()
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
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
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

GameCore::~GameCore()
{
    SDL_GL_DeleteContext(m_glContext);
    SDL_DestroyWindow(m_window);
}

bool GameCore::processInput(const SDL_Event& event)
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
}

void GameCore::update(float delta)
{
    m_gom.update(delta);
}

void GameCore::initializeOpenGL()
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

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(openglCallbackFunction, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE,
                          0, NULL, true);
}

/* function to reset our viewport after a window resize */
void GameCore::resizeWindow(int width, int height)
{
    //std::cout << "resizeWindow " << width << " " << height << std::endl;

    /* Setup our viewport. */
    glViewport(0, 0, GLsizei(width), GLsizei(height));
}

//------------------------------------------------------------------------------

void GameCore::preDraw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

//------------------------------------------------------------------------------

void GameCore::postDraw()
{
    SDL_GL_SwapWindow(m_window);
}
