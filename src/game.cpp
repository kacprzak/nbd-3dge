#include "game.h"

#include "texture.h"
#include "skybox.h"
#include "shader.h"
#include <glm/gtc/type_ptr.hpp>

Game::Game()
    : m_sp(0)
{
    init();
    loadData();
}

void Game::resizeWindow(int width, int height)
{
    /* Protect against a divide by zero */
    if (height == 0)
        height = 1;

    GLfloat ratio = GLfloat(width) / GLfloat(height);

    m_camera->setPerspective(45.0f, ratio, 5.0f, 500.0f);

    GameCore::resizeWindow(width, height);
}

void Game::init()
{
    //    // Create light components
    //    GLfloat ambientLight[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    //    GLfloat diffuseLight[] = { 0.8f, 0.8f, 0.8, 1.0f };
    //    GLfloat specularLight[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    //    GLfloat position[] = { 50.0f, 50.0f, 50.0f, 1.0f };

    //    // Assign created components to GL_LIGHT0
    //    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    //    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    //    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
    //    glLightfv(GL_LIGHT0, GL_POSITION, position);

    //    glEnable(GL_LIGHTING);
    //    glEnable(GL_LIGHT0);
}

void Game::loadData()
{
    m_camera = new Camera;
    m_camera->moveTo(0.0f, 0.0f, 80.0f);
    gameObjectManager().setCamera(m_camera);

    //MeshPtr triangleMesh(Mesh::create("data/triangle.obj", GL_FLAT));
    MeshPtr teddyMesh(Mesh::create("data/teddy2.obj"));
    MeshPtr teapotMesh(Mesh::create("data/teapot.obj"));
    //MeshPtr cowMesh(Mesh::create("data/cow-nonormals.obj"));
    MeshPtr cowMesh(Mesh::create("data/cow.obj"));
    MeshPtr cubeMesh(Mesh::create("data/cube.obj", GL_FLAT));
    MeshPtr shipMesh(Mesh::create("data/ship.obj", GL_FLAT));
    MeshPtr floorMesh(Mesh::create("data/floor.obj", GL_FLAT));

    TexturePtr shipTex(Texture::create("data/ship.jpg"));
    TexturePtr tex_I(Texture::create("data/texture_I.png"));
    TexturePtr tex_G(Texture::create("data/texture_H.png"));

    Shader *vs = new Shader(GL_VERTEX_SHADER, "shaders/shader.vert");
    Shader *fs = new Shader(GL_FRAGMENT_SHADER, "shaders/shader.frag");

    m_sp = new ShaderProgram;
    m_sp->addShared(vs);
    m_sp->addShared(fs);
    m_sp->link();

    // SKYBOX
    TexturePtr sb_front(Texture::create("data/mars/mars_front.jpg", true));
    TexturePtr sb_right(Texture::create("data/mars/mars_right.jpg", true));
    TexturePtr sb_back(Texture::create("data/mars/mars_back.jpg", true));
    TexturePtr sb_left(Texture::create("data/mars/mars_left.jpg", true));
    TexturePtr sb_top(Texture::create("data/mars/mars_top.jpg", true));
    TexturePtr sb_bottom;

    Skybox *skybox = new Skybox(sb_front, sb_right, sb_back, sb_left, sb_top, sb_bottom);
    gameObjectManager().setSkybox(skybox);

    Actor *a = new Actor("teddy", teddyMesh);
    //Actor *a = new Actor(triangleMesh);
    a->setScale(0.2f);
    gameObjectManager().add(a);

    a = new Actor("cube", cubeMesh);
    a->setTexture(tex_I);
    a->moveTo(10.0f, 0.0f, 0.0f);
    a->setScale(0.2f);
    gameObjectManager().add(a);

    a = new Actor("cow", cowMesh);
    a->moveTo(-10.0f);
    gameObjectManager().add(a);

    a = new Actor("teapot", teapotMesh);
    a->moveTo(0.0f, 10.0f);
    gameObjectManager().add(a);

    a = new Actor("ship", shipMesh);
    a->setTexture(shipTex);
    a->moveTo(0.0f, -10.0f, 0.0f);
    a->setScale(0.1f);
    gameObjectManager().add(a);

    a = new Actor("floor", floorMesh);
    a->setTexture(tex_G);
    a->setScale(10.0f);
    a->moveTo(0.0f, -15.0f, 0.0f);
    gameObjectManager().add(a);
}

void Game::draw()
{
    /* Clear The Screen And The Depth Buffer */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_sp->use();

    GLint projectionMatrixUnif = glGetUniformLocation(m_sp->id(), "projectionMatrix");
    glUniformMatrix4fv(projectionMatrixUnif, 1, GL_FALSE, glm::value_ptr(m_camera->projectionMatrix()));

    GLint viewMatrixUnif = glGetUniformLocation(m_sp->id(), "viewMatrix");
    glUniformMatrix4fv(viewMatrixUnif, 1, GL_FALSE, glm::value_ptr(m_camera->viewMatrix()));

    //super::draw();
    gameObjectManager().draw(m_sp);

    m_sp->use(false);
}

void Game::mouseWheelMoved(int wheelDelta)
{
    m_camera->moveForward(-wheelDelta);
}

void Game::update(float delta)
{
    static sf::Vector2i lastMousePos = sf::Mouse::getPosition();
    sf::Vector2i currentMousePos = sf::Mouse::getPosition();

    int dx = currentMousePos.x - lastMousePos.x;
    int dy = currentMousePos.y - lastMousePos.y;

    lastMousePos = currentMousePos;

    float mouseSensity = 0.2f;

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        m_camera->rotate(-dy * mouseSensity, -dx * mouseSensity, 0.0f);
    }

    float cameraSpeed = 50.0f;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        m_camera->moveForward(-delta * cameraSpeed);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        m_camera->moveForward(delta * cameraSpeed);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        m_camera->moveRight(-delta * cameraSpeed);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        m_camera->moveLeft(-delta * cameraSpeed);
    }

    for (Actor *a : gameObjectManager().actors()) {
        if (a->name() != "floor") {
            a->rotate(0.0f, delta * 10.0f, 0.0f);
            a->moveForward(delta * 10.0f);
        }
    }

    super::update(delta);
}
