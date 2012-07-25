#ifndef GAMEOBJECTMANAGER_H
#define GAMEOBJECTMANAGER_H

#include "actor.h"
#include "skybox.h"
#include <set>
#include "shaderprogram.h"

class GameObjectManager
{
public:
    GameObjectManager();
    ~GameObjectManager();

    void setSkybox(Skybox *skybox) {
        m_skybox = skybox;
    }

    void add(Actor *actor) {
        m_actors.insert(actor);
    }
    void remove(Actor *actor) {
        m_actors.erase(actor);
    }

    const std::set<Actor *>& actors() const {
        return m_actors;
    }

    void draw() const;
    void draw(ShaderProgram *program) const;
    void update(float delta);

private:
    std::set<Actor *> m_actors;
    Skybox *m_skybox;
};

#endif // GAMEOBJECTMANAGER_H
