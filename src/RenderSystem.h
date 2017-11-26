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
    RenderSystem(Texture::Size windowSize);
    ~RenderSystem();
    RenderSystem(const RenderSystem&) = delete;
    RenderSystem& operator=(const RenderSystem&) = delete;

    void loadCommonResources(const ResourcesMgr& resourcesMgr);

    void addActor(int id, TransformationComponent* tr, RenderComponent* rd, LightComponent* lt,
                  const ResourcesMgr& resourcesMgr);
    void removeActor(int id);

    void draw();
    void update(float delta);
    void setNextPolygonMode();

    void setDrawNormals(bool enable, float normalLength = 1.0f);
    bool isDrawNormals() const { return m_drawNormals; }

    Camera* getCamera() { return m_camera.get(); }

    void resizeWindow(Texture::Size size);

  private:
    void setSkybox(std::shared_ptr<Skybox> skybox) { m_skybox = skybox; }

    void setCamera(std::shared_ptr<Camera> camera) { m_camera = camera; }

    void add(std::shared_ptr<Text> actor) { m_texts.insert(actor); }
    void remove(std::shared_ptr<Text> actor) { m_texts.erase(actor); }

    void draw(const Camera* camera, std::array<Light*, 8>& lights) const;
    void draw(ShaderProgram* shaderProgram, const Camera* camera,
              std::array<Light*, 8>& lights) const;

    std::set<ShaderProgram*> getShaders() const;

    Aabb calcDirectionalLightProjection(const Light& light) const;

    GLenum m_polygonMode = GL_FILL;
    Texture::Size m_windowSize;

    std::map<int, std::shared_ptr<RenderNode>> m_nodes;
    std::map<int, std::shared_ptr<RenderNode>> m_transparentNodes;
    std::map<int, std::shared_ptr<Light>> m_lights;

    std::shared_ptr<Skybox> m_skybox;
    std::shared_ptr<Camera> m_camera;
    std::shared_ptr<Text> m_cameraText;
    std::set<std::shared_ptr<Text>> m_texts;

    std::shared_ptr<ShaderProgram> m_normalsShader;
    std::shared_ptr<ShaderProgram> m_shadowShader;

    Texture::Size m_shadowMapSize;
    std::unique_ptr<Framebuffer> m_shadowMapFB;

    bool m_drawNormals = false;
    FpsCounter m_fpsCounter;
};

#endif // RENDERSYSTEM_H
