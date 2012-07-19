#include "gametest1.h"

#include <boost/shared_ptr.hpp>
#include "texture.h"

void GameTest1::loadData()
{
    using namespace boost;

    shared_ptr<Mesh> teddyMesh(Mesh::create("data/teddy.obj"));
    shared_ptr<Mesh> teapotMesh(Mesh::create("data/teapot.obj"));
    shared_ptr<Mesh> cowMesh(Mesh::create("data/cow-nonormals.obj"));
    shared_ptr<Mesh> cube(Mesh::create("data/cube.obj", GL_FLAT));
    shared_ptr<Mesh> ship(Mesh::create("data/ship.obj", GL_FLAT));

    shared_ptr<Texture> tex1(Texture::create("data/ship.jpg"));
    shared_ptr<Texture> tex2(Texture::create("data/texture_I.png"));

    Actor *a = new Actor(teddyMesh);
    a->setScale(0.2f);
    m_gom.add(a);

    a = new Actor(cube);
    a->setTexture(tex2);
    a->setScale(0.2f);
    a->moveTo(Vector<float>(10.0f));
    m_gom.add(a);

    a = new Actor(cowMesh);
    a->moveTo(Vector<float>(-10.0f));
    m_gom.add(a);

    a = new Actor(teapotMesh);
    a->moveTo(Vector<float>(0.0f, 10.0f));
    m_gom.add(a);

    a = new Actor(ship);
    a->setTexture(tex1);
    a->setScale(0.1f);
    a->moveTo(Vector<float>(0.0f, -10.0f));
    m_gom.add(a);
}
