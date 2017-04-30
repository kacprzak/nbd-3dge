#ifndef GAMEOBJECTMANAGER_H
#define GAMEOBJECTMANAGER_H

#include "GfxNode.h"
#include "Skybox.h"
#include <set>
#include "ShaderProgram.h"
#include "Camera.h"
#include "Text.h"

class GfxScene
{
public:
    GfxScene();
    ~GfxScene();

    void setSkybox(std::shared_ptr<Skybox> skybox) {
        m_skybox = skybox;
    }

    void setCamera(std::shared_ptr<Camera> camera) {
        m_camera = camera;
    }

    void add(std::shared_ptr<GfxNode> actor) {
        m_actors.insert(actor);
    }
    void remove(std::shared_ptr<GfxNode> actor) {
        m_actors.erase(actor);
    }

    void add(std::shared_ptr<Text> actor) {
        m_texts.insert(actor);
    }
    void remove(std::shared_ptr<Text> actor) {
        m_texts.erase(actor);
    }
    
    const std::set<std::shared_ptr<GfxNode>>& actors() const {
        return m_actors;
    }

    void draw(const Camera* camera) const;
    void draw(ShaderProgram* shaderProgram, const Camera* camera) const;

    void update(float delta);

private:
    std::set<std::shared_ptr<GfxNode>> m_actors;
    std::shared_ptr<Skybox> m_skybox;
    std::shared_ptr<Camera> m_camera;
    std::set<std::shared_ptr<Text>> m_texts;
};

#endif // GAMEOBJECTMANAGER_H
