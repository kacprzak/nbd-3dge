#include "game.h"

#include <boost/shared_ptr.hpp>
#include "texture.h"
#include "skybox.h"

Game::Game()
{
    m_zoom = 40.0f;
    m_elevation = 0.0f;
    m_azimuth = 0.0f; 
  
    init();
    loadData();
}

void Game::init()
{
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

void Game::loadData()
{
    using namespace boost;

    shared_ptr<Mesh> teddyMesh(Mesh::create("data/teddy.obj"));
    //shared_ptr<Mesh> teapotMesh(Mesh::create("data/teapot.obj"));
    //shared_ptr<Mesh> cowMesh(Mesh::create("data/cow-nonormals.obj"));
    shared_ptr<Mesh> cube(Mesh::create("data/cube.obj", GL_FLAT));
    shared_ptr<Mesh> ship(Mesh::create("data/ship.obj", GL_FLAT));

    shared_ptr<Texture> tex1(Texture::create("data/ship.jpg"));
    shared_ptr<Texture> tex2(Texture::create("data/texture_I.png"));
    shared_ptr<Texture> skybox_tex(Texture::create("data/skybox_texture.jpg"));

    Actor *a = new SkyBox();
    a->setTexture(skybox_tex);
    m_gom.add(a);

    a = new Actor(teddyMesh);
    a->setScale(0.2f);
    m_gom.add(a);

    a = new Actor(cube);
    a->setTexture(tex2);
    a->setScale(0.2f);
    a->moveTo({10.0f});
    m_gom.add(a);

//     a = new Actor(cowMesh);
//     a->moveTo({-10.0f});
//     m_gom.add(a);
// 
//     a = new Actor(teapotMesh);
//     a->moveTo({0.0f, 10.0f});
//     m_gom.add(a);

    a = new Actor(ship);
    a->setTexture(tex1);
    a->setScale(0.1f);
    a->moveTo({0.0f, -10.0f});
    m_gom.add(a);
}

void Game::polarView()
{
    glTranslatef(0.0, 0.0, -m_zoom);
    glRotatef(-m_elevation, 1.0f, 0.0f, 0.0f);
    glRotatef(m_azimuth, 0.0f, 1.0f, 0.0f);
}

void Game::draw()
{
    /* Clear The Screen And The Depth Buffer */
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glLoadIdentity();
    polarView();  
  
    super::draw();
}

void Game::mouseWheelMoved(int wheelDelta)
{
    m_zoom -= wheelDelta;  
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
  
    super::update(delta);
}
