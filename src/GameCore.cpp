#include "GameCore.h"

#include <iostream>
#include "FpsCounter.h"

#ifdef _WIN32
#include <windows.h>
#else
#define APIENTRY
#endif

void printOpenGlSettings(const sf::Window& window)
{
    sf::ContextSettings settings = window.getSettings();

    std::cout << "OpenGL settings:" << std::endl;
    std::cout << "  depth bits: " << settings.depthBits << std::endl;
    std::cout << "  stencil bits: " << settings.stencilBits << std::endl;
    std::cout << "  antialiasing level: " << settings.antialiasingLevel << std::endl;
    std::cout << "  version: " << settings.majorVersion << "."
              << settings.minorVersion << std::endl;
}

static void APIENTRY openglCallbackFunction(GLenum source,
                                            GLenum type,
                                            GLuint id,
                                            GLenum severity,
                                            GLsizei length,
                                            const GLchar* message,
                                            const void* userParam)
{
    if (severity != GL_DEBUG_SEVERITY_NOTIFICATION) {
        fprintf(stderr, "%s\n", message);
        if (severity == GL_DEBUG_SEVERITY_HIGH) {
            fprintf(stderr, "Aborting...\n");
            abort();
        }
    }
}

GameCore::GameCore()
{
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 4;
    settings.majorVersion = 4;
    settings.minorVersion = 3;
    settings.attributeFlags = sf::ContextSettings::Core;
#ifndef NDEBUG
    settings.attributeFlags |= sf::ContextSettings::Debug;
#endif
    
    m_window = new sf::Window(sf::VideoMode(800, 600), "nbd-3dge",
                              sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize,
                              settings);
    m_window->setVerticalSyncEnabled(true);
    m_window->setFramerateLimit(60);

    initGL();

#ifndef NDEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(openglCallbackFunction, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE,
                          0, NULL, true);
#endif

    printOpenGlSettings(*m_window);
}

GameCore::~GameCore()
{
    delete m_window;
}

void GameCore::mainLoop()
{
    float delta = 0.0f;
    FpsCounter fpsCounter;
    sf::Clock clock;
    bool running = true;
    bool paused = false;

    while (running) {
        delta = clock.restart().asSeconds();
        //std::cout << delta << "\n";
        fpsCounter.update(delta);

        sf::Event event;
        while (m_window->pollEvent(event)) {
            switch (event.type) {
            case sf::Event::Closed:
                running = false;
                break;
            case sf::Event::Resized:
                resizeWindow(event.size.width, event.size.height);
                break;
            case sf::Event::LostFocus:
                paused = true;
                break;
            case sf::Event::GainedFocus:
                paused = false;
                break;
            case sf::Event::MouseWheelMoved:
                mouseWheelMoved(event.mouseWheel.delta);
                break;
            case sf::Event::KeyPressed:
                keyPressed(event.key);
                break;
            case sf::Event::KeyReleased:
                keyReleased(event.key);
                break;
            default:
                break;
            }
        }

        if (!paused)
            update(delta);
        // Draw to the back buffer
        draw();
        // Swap buffers
        m_window->display();
    }
}

void GameCore::update(float delta)
{
    m_gom.update(delta);
}

void GameCore::initGL()
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

    glEnable(GL_CULL_FACE);
    //glEnable(GL_RESCALE_NORMAL);

    //glEnable(GL_TEXTURE_2D);
    /* Gamma correction */
    glEnable(GL_FRAMEBUFFER_SRGB);
}

/* function to reset our viewport after a window resize */
void GameCore::resizeWindow(int width, int height)
{
    //std::cout << "resizeWindow " << width << " " << height << std::endl;

    /* Setup our viewport. */
    glViewport(0, 0, GLsizei(width), GLsizei(height));
}
