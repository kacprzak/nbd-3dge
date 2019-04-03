#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "Aabb.h"
#include "Components.h"
#include "Scene.h"
#include "ShaderProgram.h"
#include "Text.h"
#include "Camera.h"

#include <map>
#include <set>

class ResourcesMgr;
class RenderNode;
class Light;
class Framebuffer;
class Skybox;

class RenderSystem final
{
  public:
    enum CameraType { Player, Free };

    RenderSystem(glm::ivec2 windowSize);
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

    Camera* getCamera() { return m_camera; }
    Camera* getCamera(CameraType type) { return &m_cameras.at(type); }

    RenderNode* findNode(const std::string& node);

    void setCamera(CameraType type) { m_camera = &m_cameras.at(type); }
    void setScene(std::shared_ptr<Scene> scene);

    void resizeWindow(glm::ivec2 size);

  private:
    void setSkybox(std::shared_ptr<Skybox> skybox) { m_skybox = skybox; }

    void add(std::shared_ptr<Text> actor) { m_texts.insert(actor); }
    void remove(std::shared_ptr<Text> actor) { m_texts.erase(actor); }

    void draw(const Camera* camera, std::array<Light*, 8>& lights) const;

    void drawNormals(ShaderProgram* shaderProgram, const Camera* camera) const;
    void drawShadows(ShaderProgram* shaderProgram, Camera* camera, Light* light) const;
    void drawAabb(ShaderProgram* shaderProgram, const Camera* camera) const;
    void drawFrustum(ShaderProgram* shaderProgram, const Camera* camera) const;

    std::set<ShaderProgram*> getShaders() const;

    Aabb calcDirectionalLightProjection(const Camera& camera, const Camera& light,
                                        int cascadeIndex) const;
    void updateCameraText();

    GLenum m_polygonMode = GL_FILL;
    GLuint m_emptyVao    = 0; // For drawing with no data

    int m_shadowCascadesSize;
    glm::ivec2 m_windowSize;

    std::map<int, std::shared_ptr<RenderNode>> m_nodes;
    std::map<int, std::shared_ptr<RenderNode>> m_transparentNodes;
    std::map<int, std::shared_ptr<Light>> m_lights;
    std::vector<Camera> m_cameras;
    Camera* m_camera; // current camera

    std::shared_ptr<Scene> m_scene;
    std::shared_ptr<Skybox> m_skybox;
    std::shared_ptr<Text> m_cameraText;
    std::set<std::shared_ptr<Text>> m_texts;

    std::shared_ptr<ShaderProgram> m_defaultShader;
    std::shared_ptr<ShaderProgram> m_shadowShader;
    std::shared_ptr<ShaderProgram> m_normalsShader;
    std::shared_ptr<ShaderProgram> m_aabbShader;
    std::shared_ptr<ShaderProgram> m_frustumShader;

    glm::ivec3 m_shadowMapSize;
    std::unique_ptr<Framebuffer> m_shadowMapFB;

    bool m_drawNormals = false;
};

#endif // RENDERSYSTEM_H
