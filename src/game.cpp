#include "game.h"

#include <boost/shared_ptr.hpp>
#include "texture.h"
#include "skybox.h"
#include "shader.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

Game::Game()
    : m_sp(0)
    , m_viewMatrix(glm::mat4(1.0f))
{
    m_zoom = 40.0f;
    m_elevation = 0.0f;
    m_azimuth = 0.0f;

    init();
    loadData();
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
    using namespace boost;

    //shared_ptr<Mesh> triangleMesh(Mesh::create("data/triangle.obj", GL_FLAT));
    shared_ptr<Mesh> teddyMesh(Mesh::create("data/teddy.obj"));
    //shared_ptr<Mesh> teapotMesh(Mesh::create("data/teapot.obj"));
    //shared_ptr<Mesh> cowMesh(Mesh::create("data/cow-nonormals.obj"));
    shared_ptr<Mesh> cube(Mesh::create("data/cube.obj", GL_FLAT));
    shared_ptr<Mesh> ship(Mesh::create("data/ship.obj", GL_FLAT));

    //shared_ptr<Texture> tex1(Texture::create("data/ship.jpg"));
    //shared_ptr<Texture> tex2(Texture::create("data/texture_I.png"));
    //shared_ptr<Texture> skybox_tex(Texture::create("data/skybox_texture.jpg"));

    Shader *vs = new Shader(GL_VERTEX_SHADER, "shaders/shader.vert");
    Shader *fs = new Shader(GL_FRAGMENT_SHADER, "shaders/shader.frag");

    m_sp = new ShaderProgram;
    m_sp->addShared(vs);
    m_sp->addShared(fs);
    m_sp->link();

//    Skybox *skybox = new Skybox(skybox_tex);
//    m_gom.setSkybox(skybox);

    Actor *a = new Actor(teddyMesh);
    //Actor *a = new Actor(triangleMesh);
    a->setScale(0.2f);
    m_gom.add(a);

    a = new Actor(cube);
//    a->setTexture(tex2);
    a->moveTo(glm::vec3(10.0f, 0.0f, 0.0f));
    a->setScale(0.2f);
    m_gom.add(a);

//     a = new Actor(cowMesh);
//     a->moveTo({-10.0f});
//     m_gom.add(a);
// 
//     a = new Actor(teapotMesh);
//     a->moveTo({0.0f, 10.0f});
//     m_gom.add(a);

    a = new Actor(ship);
//    a->setTexture(tex1);
    a->moveTo(glm::vec3(0.0f, -10.0f, 0.0f));
    a->setScale(0.1f);
    m_gom.add(a);
}

void Game::polarView()
{
    m_viewMatrix = glm::mat4(1.0f);
    m_viewMatrix = glm::translate(m_viewMatrix, glm::vec3(0.0f, 0.0f, -m_zoom));
    m_viewMatrix = glm::rotate(m_viewMatrix, -m_elevation, glm::vec3(1.0f, 0.0f, 0.0f));
    m_viewMatrix = glm::rotate(m_viewMatrix, m_azimuth, glm::vec3(0.0f, 1.0f, 0.0f));
}

void Game::draw()
{
    /* Clear The Screen And The Depth Buffer */
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    polarView();

    m_sp->use();

    GLint projectionMatrixUnif = glGetUniformLocation(m_sp->id(), "projectionMatrix");
    glUniformMatrix4fv(projectionMatrixUnif, 1, GL_FALSE, glm::value_ptr(m_projectionMatrix));

    GLint viewMatrixUnif = glGetUniformLocation(m_sp->id(), "viewMatrix");
    glUniformMatrix4fv(viewMatrixUnif, 1, GL_FALSE, glm::value_ptr(m_viewMatrix));

    //super::draw();
    m_gom.draw(m_sp);

    m_sp->use(false);
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

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        m_azimuth += dx * mouseSensity;
        m_elevation += dy * mouseSensity;
    }

    super::update(delta);
}
