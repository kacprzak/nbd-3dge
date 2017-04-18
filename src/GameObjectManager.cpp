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

void GameObjectManager::draw(const Camera* camera) const
{
    if (m_skybox)
        m_skybox->draw(camera);

    if (m_camera)
        m_camera->draw(camera);

    for (Actor *a : m_actors) {
        a->draw(camera);
    }
}

void GameObjectManager::update(float delta)
{  
    if (m_camera) {
        m_camera->update(delta);
    }

    for (Actor *a : m_actors) {
        a->update(delta);
    }
}
