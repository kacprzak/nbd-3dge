#include "gameobjectmanager.h"

GameObjectManager::GameObjectManager()
    : m_skybox(0)
{
}

GameObjectManager::~GameObjectManager()
{
    if (m_skybox)
        delete m_skybox;

    for (Actor *a : m_actors) {
        delete a;
    }
    m_actors.clear();
}

void GameObjectManager::draw()
{
    if (m_skybox)
        m_skybox->draw();

    for (Actor *a : m_actors) {
        a->draw();
    }
}

void GameObjectManager::draw(ShaderProgram *program)
{
    if (m_skybox)
        m_skybox->draw();

    for (Actor *a : m_actors) {
        a->draw(program);
    }
}

void GameObjectManager::update(float delta)
{
    for (Actor *a : m_actors) {
        a->update(delta);
    }
}
