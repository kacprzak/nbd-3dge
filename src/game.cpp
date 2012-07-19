#include "game.h"

#include <iostream>
//#include <vector>
#include "fpscounter.h"

void printOpenGlSettings(const sf::Window& window)
{
    sf::ContextSettings settings = window.getSettings();

    std::cout << "OpenGL settings:" << std::endl;
    std::cout << "  depth bits: " << settings.depthBits << std::endl;
    std::cout << "  stencil bits: " << settings.stencilBits << std::endl;
    std::cout << "  antialiasing level: " << settings.antialiasingLevel << std::endl;
    std::cout << "  version: " << settings.majorVersion << "." << settings.minorVersion << std::endl;
}

Game::Game()
{
    m_window = new sf::Window(sf::VideoMode(800, 600), "nbd-3dge",
                              sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize);

    initGL();
    printOpenGlSettings(*m_window);

    m_zoom = 20.0f;
    m_elevation = 0.0f;
    m_azimuth = 0.0f;
}

Game::~Game()
{
    delete m_window;
}

void Game::mainLoop()
{
    float delta = 0.0f;
    FpsCounter fpsCounter;
    sf::Clock clock;
    bool running = true;

    while (running)
    {
        delta = clock.restart().asSeconds();
        fpsCounter.update(delta);

        sf::Event event;
        while (m_window->pollEvent(event))
        {
            switch (event.type) {
            case sf::Event::Closed:
                running = false;
                break;
            case sf::Event::Resized:
                resizeWindow(event.size.width, event.size.height);
                break;
            case sf::Event::MouseWheelMoved:
                mouseWheelMoved(event.mouseWheel.delta);
                break;
            default:
                break;
            }
        }

        update(delta);
        // Draw to the back buffer
        draw();
        // Swap buffers
        m_window->display();
    }
}

void Game::draw()
{
    /* Clear The Screen And The Depth Buffer */
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glLoadIdentity();
    polarView();

    m_gom.draw();
}

void Game::update(float delta)
{
    static sf::Vector2i lastMousePos = sf::Mouse::getPosition();
    sf::Vector2i currentMousePos = sf::Mouse::getPosition();

    int dx = currentMousePos.x - lastMousePos.x;
    int dy = currentMousePos.y - lastMousePos.y;

    lastMousePos = currentMousePos;

    float mouseSensity = 0.2f;

    //float yAngle = 0.0f;
    //float xAngle = 0.0f;

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        m_azimuth += dx * mouseSensity;
        m_elevation += dy * mouseSensity;
    }

    m_gom.update(delta);
}

void Game::mouseWheelMoved(int wheelDelta)
{
    m_zoom -= wheelDelta;
}

void Game::polarView()
{
    glTranslatef(0.0, 0.0, -m_zoom);
    glRotatef(-m_elevation, 1.0f, 0.0f, 0.0f);
    glRotatef(m_azimuth, 0.0f, 1.0f, 0.0f);
}

void Game::initGL()
{
    glewInit();

    /* Enable smooth shading */
    //glShadeModel( GL_SMOOTH );
    //glShadeModel( GL_FLAT );

    /* Set the background black */
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

    /* Depth buffer setup */
    glClearDepth( 1.0f );

    /* Enables Depth Testing */
    glEnable( GL_DEPTH_TEST );

    /* The Type Of Depth Test To Do */
    glDepthFunc( GL_LEQUAL );

    /* Really Nice Perspective Calculations */
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

    //glEnable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);
    glEnable(GL_RESCALE_NORMAL);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    // Light

    // Create light components
    GLfloat ambientLight[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat diffuseLight[] = { 0.8f, 0.8f, 0.8, 1.0f };
    GLfloat specularLight[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    GLfloat position[] = { 50.0f, 50.0f, 50.0f, 1.0f };

    // Assign created components to GL_LIGHT0
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
    glLightfv(GL_LIGHT0, GL_POSITION, position);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
}

/* function to reset our viewport after a window resize */
bool Game::resizeWindow(int width, int height)
{
    /* Height / width ration */
    GLfloat ratio;

    /* Protect against a divide by zero */
    if (height == 0)
        height = 1;

    ratio = GLfloat(width) / GLfloat(height);

    /* Setup our viewport. */
    glViewport(0, 0, GLsizei(width), GLsizei(height));

    /* change to the projection matrix and set our viewing volume. */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    /* Set our perspective */
    gluPerspective(45.0f, ratio, 0.1f, 100.0f);
    //gluOrtho2D(0.0f, 0.0f, GLdouble(width), GLdouble(height));

    /* Make sure we're chaning the model view and not the projection */
    glMatrixMode(GL_MODELVIEW);

    /* Reset The View */
    glLoadIdentity();

    return true;
}
