#ifndef RENDERNODE_H
#define RENDERNODE_H

#include "Components.h"
//#include "Light.h"
#include "Material.h"
#include "Mesh.h"
#include "ShaderProgram.h"
//#include "Texture.h"

#include <glm/glm.hpp>

class Camera;
class Light;

class RenderNode
{
  public:
    RenderNode(int actorId, TransformationComponent* tr, RenderComponent* rd);
    virtual ~RenderNode() = default;

    // void setTextures(const std::vector<std::shared_ptr<Texture>>& textures);
    // void addTexture(const std::shared_ptr<Texture>& texture);

    void setMaterial(const std::shared_ptr<Material>& material);

    void setMesh(const std::shared_ptr<Mesh>& mesh);
    void setShaderProgram(const std::shared_ptr<ShaderProgram>& shaderProgram);

    ShaderProgram* getShaderProgram() { return m_shaderProgram.get(); }

    virtual void draw(const Camera* camera, const std::array<Light*, 8>& lights,
                      Texture* environment) const;
    virtual void draw(ShaderProgram* shaderProgram, const Camera* camera,
                      const std::array<Light*, 8>& lights, Texture* environment) const;

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
    // std::vector<std::shared_ptr<Texture>> m_textures;

    bool m_castsShadows = false;
};

#endif // RENDERNODE_H
