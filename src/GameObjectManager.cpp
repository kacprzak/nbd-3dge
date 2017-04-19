#include "GameObjectManager.h"

GameObjectManager::GameObjectManager()
{
}

GameObjectManager::~GameObjectManager()
{
}

void GameObjectManager::draw(const Camera* camera) const
{
    if (m_skybox)
        m_skybox->draw(camera);

    if (m_camera)
        m_camera->draw(camera);

    for (const auto& a : m_actors) {
        a->draw(camera);
    }
}

void GameObjectManager::update(float delta)
{  
    if (m_camera) {
        m_camera->update(delta);
    }

    for (const auto& a : m_actors) {
        a->update(delta);
    }
}
