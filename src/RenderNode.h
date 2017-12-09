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
    RenderNode(int actorId, TransformationComponent* tr, RenderComponent* rd);

    RenderNode(const RenderNode&) = delete;
    RenderNode& operator=(const RenderNode&) = delete;

    RenderNode(RenderNode&& other);

    virtual ~RenderNode() = default;

    void setMaterial(const std::shared_ptr<Material>& material);

    void setMesh(const std::shared_ptr<Mesh>& mesh);
    void setShaderProgram(const std::shared_ptr<ShaderProgram>& shaderProgram);

    ShaderProgram* getShaderProgram() { return m_shaderProgram.get(); }

    virtual void draw(const Camera* camera, const std::array<Light*, 8>& lights,
                      Texture* environment) const;
    virtual void draw(ShaderProgram* shaderProgram, const Camera* camera,
                      const std::array<Light*, 8>& lights, Texture* environment) const;

    virtual void drawAabb(ShaderProgram* shaderProgram, const Camera* camera);

    virtual void update(float delta);

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

  protected:
    const glm::mat4& modelMatrix() const { return m_modelMatrix; }

    const int m_actorId;
    TransformationComponent* m_tr = nullptr;
    RenderComponent* m_rd         = nullptr;

    std::shared_ptr<Material> m_material;

  private:
    void rebuildModelMatrix();

    glm::mat4 m_modelMatrix;

    std::shared_ptr<Mesh> m_mesh;
    std::shared_ptr<ShaderProgram> m_shaderProgram;

    bool m_castsShadows = false;
};

#endif // RENDERNODE_H
