#include "GameObjectManager.h"

GameObjectManager::GameObjectManager()
    : m_skybox(0)
    , m_camera(0)
{
}

GameObjectManager::~GameObjectManager()
{
    if (m_skybox)
        delete m_skybox;

    if (m_camera)
        delete m_camera;

    for (Actor *a : m_actors) {
        delete a;
    }
    m_actors.clear();
}

void GameObjectManager::draw() const
{
    if (m_skybox)
        m_skybox->draw();

    for (Actor *a : m_actors) {
        a->draw();
    }
}

void GameObjectManager::draw(ShaderProgram *program) const
{
    if (m_skybox)
        m_skybox->draw(program);

    for (Actor *a : m_actors) {
        a->draw(program);
    }
}

void GameObjectManager::update(float delta)
{
    if (m_camera && m_skybox) {
        m_skybox->moveTo(m_camera->position());
    }

    for (Actor *a : m_actors) {
        a->update(delta);
    }
}
