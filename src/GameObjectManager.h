#ifndef GAMEOBJECTMANAGER_H
#define GAMEOBJECTMANAGER_H

#include "Actor.h"
#include "Skybox.h"
#include <set>
#include "ShaderProgram.h"
#include "Camera.h"

class GameObjectManager
{
public:
    GameObjectManager();
    ~GameObjectManager();

    void setSkybox(std::shared_ptr<Skybox> skybox) {
        m_skybox = skybox;
    }

    void setCamera(std::shared_ptr<Camera> camera) {
        m_camera = camera;
    }

    void add(std::shared_ptr<Actor> actor) {
        m_actors.insert(actor);
    }
    void remove(std::shared_ptr<Actor> actor) {
        m_actors.erase(actor);
    }

    const std::set<std::shared_ptr<Actor>>& actors() const {
        return m_actors;
    }

    void draw(const Camera* camera) const;
    void update(float delta);

private:
    std::set<std::shared_ptr<Actor>> m_actors;
    std::shared_ptr<Skybox> m_skybox;
    std::shared_ptr<Camera> m_camera;
};

#endif // GAMEOBJECTMANAGER_H
