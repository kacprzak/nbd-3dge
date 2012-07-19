#include "gameobjectmanager.h"

GameObjectManager::GameObjectManager()
{
}

GameObjectManager::~GameObjectManager()
{
    for (Actor *a : m_actors) {
        delete a;
    }
    m_actors.clear();
}

void GameObjectManager::draw()
{
    for (Actor *a : m_actors) {
        a->draw();
    }
}

void GameObjectManager::update(float delta)
{
    for (Actor *a : m_actors) {
        a->update(delta);
    }
}
