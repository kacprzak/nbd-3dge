#ifndef GAMEOBJECTMANAGER_H
#define GAMEOBJECTMANAGER_H

#include "actor.h"
#include <set>

class GameObjectManager
{
public:
    GameObjectManager();
    ~GameObjectManager();

    void add(Actor *actor) {
        m_actors.insert(actor);
    }
    void remove(Actor *actor) {
        m_actors.erase(actor);
    }

    void draw();
    void update(float delta);

private:
    std::set<Actor *> m_actors;
};

#endif // GAMEOBJECTMANAGER_H
