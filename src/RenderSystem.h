#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "Camera.h"
#include "FpsCounter.h"
#include "Framebuffer.h"
#include "Light.h"
#include "RenderNode.h"
#include "ShaderProgram.h"
#include "Skybox.h"
#include "Text.h"

#include <map>
#include <set>

class ResourcesMgr;

class RenderSystem final
{
  public:
    RenderSystem();
    ~RenderSystem();
    RenderSystem(const RenderSystem&) = delete;
    RenderSystem& operator=(const RenderSystem&) = delete;

    void loadCommonResources(const ResourcesMgr& resourcesMgr);

    void addActor(int id, TransformationComponent* tr, RenderComponent* rd,
                  const ResourcesMgr& resourcesMgr);
    void removeActor(int id);

    void draw();
    void update(float delta);
    void setNextPolygonMode();

    void setDrawNormals(bool enable, float normalLength = 1.0f);
    bool isDrawNormals() const { return m_drawNormals; }

    Camera* getCamera() { return m_camera.get(); }

  private:
    void setSkybox(std::shared_ptr<Skybox> skybox) { m_skybox = skybox; }

    void setCamera(std::shared_ptr<Camera> camera) { m_camera = camera; }

    void add(int id, std::shared_ptr<RenderNode> actor) { m_nodes[id] = actor; }
    void remove(int id) { m_nodes.erase(id); }

    void addTransparent(int id, std::shared_ptr<RenderNode> actor)
    {
        m_transparentNodes[id] = actor;
    }
    void removeTransparent(int id) { m_transparentNodes.erase(id); }

    void add(std::shared_ptr<Text> actor) { m_texts.insert(actor); }
    void remove(std::shared_ptr<Text> actor) { m_texts.erase(actor); }

    void draw(const Camera* camera, std::array<Light*, 8>& lights) const;
    void draw(ShaderProgram* shaderProgram, const Camera* camera,
              std::array<Light*, 8>& lights) const;

    GLenum m_polygonMode = GL_FILL;

    std::map<int, std::shared_ptr<RenderNode>> m_nodes;
    std::map<int, std::shared_ptr<RenderNode>> m_transparentNodes;
    std::shared_ptr<Skybox> m_skybox;
    std::shared_ptr<Camera> m_camera;
    std::shared_ptr<Text> m_cameraText;
    std::set<std::shared_ptr<Text>> m_texts;

    std::shared_ptr<ShaderProgram> m_normalsShader;
    std::unique_ptr<Framebuffer> m_shadowMapFB;

    bool m_drawNormals = false;
    FpsCounter m_fpsCounter;
};

#endif // RENDERSYSTEM_H
