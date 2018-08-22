#ifndef RENDERNODE_H
#define RENDERNODE_H

#include "Components.h"
#include "Mesh.h"
#include "ShaderProgram.h"

#include <glm/glm.hpp>

#include <memory>

class Camera;
class Light;
class Material;
class Texture;

class RenderNode
{
  public:
    explicit RenderNode(int actorId, TransformationComponent* tr = nullptr, RenderComponent* rd = nullptr);

    RenderNode(const RenderNode&) = delete;
    RenderNode& operator=(const RenderNode&) = delete;

    RenderNode(RenderNode&& other) = default;

    virtual ~RenderNode() = default;

    void setMesh(const std::shared_ptr<Mesh>& mesh);
    void setShaderProgram(const std::shared_ptr<ShaderProgram>& shaderProgram);

    void setTranslation(glm::vec3 translation) { m_translation = translation; }
    glm::vec3 getTranslation() const { return m_translation; }

    void setRotation(glm::quat rotation) { m_rotation = rotation; }
    glm::quat getRotation() const  { return m_rotation; }

    void setScale(glm::vec3 scale) { m_scale = scale; }
    glm::vec3 getScale() const { return m_scale; }

    ShaderProgram* getShaderProgram() { return m_shaderProgram.get(); }

    virtual void draw(const glm::mat4& parentModelMatrix, const Camera* camera,
                      const std::array<Light*, 8>& lights, Texture* environment) const;
    virtual void draw(const glm::mat4& parentModelMatrix, ShaderProgram* shaderProgram,
                      const Camera* camera, const std::array<Light*, 8>& lights,
                      Texture* environment) const;

    virtual void drawAabb(const glm::mat4& parentModelMatrix, ShaderProgram* shaderProgram,
                          const Camera* camera);

    virtual void update(const glm::mat4& parentModelMatrix, float delta);

    TransformationComponent* transformation() { return m_tr; }
    const TransformationComponent* transformation() const { return m_tr; }

    RenderComponent* render() { return m_rd; }
    const RenderComponent* render() const { return m_rd; }

    Aabb aabb() const
    {
        if (m_mesh)
            return m_modelMatrix * m_mesh->aabb();
        else
            return Aabb{};
    }

    void setCastShadows(bool castsShadows) { m_castsShadows = castsShadows; }
    bool castsShadows() const { return m_castsShadows; }

    void addChild(RenderNode* node) { m_children.push_back(node); }

    void removeChild(RenderNode* node)
    {
        m_children.erase(std::remove(m_children.begin(), m_children.end(), node), m_children.end());
    }

    void setCamera(Camera* camera) { m_camera = camera; }
    void removeCamera() { m_camera = nullptr; }

    std::string name;
  protected:
    const glm::mat4& modelMatrix() const { return m_modelMatrix; }

    const int m_actorId;

    TransformationComponent* m_tr = nullptr;
    RenderComponent* m_rd         = nullptr;

  private:
    void rebuildModelMatrix();

    glm::quat m_rotation{1.f, 0.f, 0.f, 0.f};
    glm::vec3 m_translation{};
    glm::vec3 m_scale{1.0f, 1.0f, 1.0f};

    glm::mat4 m_modelMatrix{1.0f};

    std::vector<RenderNode*> m_children;
    Camera* m_camera = nullptr;

    std::shared_ptr<Mesh> m_mesh;
    std::shared_ptr<ShaderProgram> m_shaderProgram;

    bool m_castsShadows = false;
};

#endif // RENDERNODE_H
