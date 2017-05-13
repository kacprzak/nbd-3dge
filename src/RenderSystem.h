#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "Camera.h"
#include "RenderNode.h"
#include "ShaderProgram.h"
#include "Skybox.h"
#include "Text.h"

#include <map>
#include <set>

class RenderSystem
{
  public:
    RenderSystem();
    ~RenderSystem();

    void setSkybox(std::shared_ptr<Skybox> skybox) { m_skybox = skybox; }

    void setCamera(std::shared_ptr<Camera> camera) { m_camera = camera; }

    void add(int id, std::shared_ptr<RenderNode> actor) { m_nodes[id] = actor; }
    void remove(int id) { m_nodes.erase(id); }

    void add(std::shared_ptr<Text> actor) { m_texts.insert(actor); }
    void remove(std::shared_ptr<Text> actor) { m_texts.erase(actor); }

    void draw(const Camera* camera) const;
    void draw(ShaderProgram* shaderProgram, const Camera* camera) const;

    void update(float delta);

    void setNextPolygonMode();

  private:
    GLenum m_polygonMode = GL_FILL;

    std::map<int, std::shared_ptr<RenderNode>> m_nodes;
    std::shared_ptr<Skybox> m_skybox;
    std::shared_ptr<Camera> m_camera;
    std::set<std::shared_ptr<Text>> m_texts;
};

#endif // RENDERSYSTEM_H
