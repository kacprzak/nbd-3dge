#ifndef RENDERNODE_H
#define RENDERNODE_H

#include "Components.h"
#include "Mesh.h"
#include "ShaderProgram.h"
#include "Texture.h"

#include <glm/glm.hpp>

class Camera;

class RenderNode
{
  public:
    RenderNode(int actorId, TransformationComponent* tr, RenderComponent* rd);
    ~RenderNode() = default;

    void setTextures(std::vector<std::shared_ptr<Texture>> textures);
    void addTexture(std::shared_ptr<Texture> texture);

    void setMesh(std::shared_ptr<Mesh> mesh);
    void setShaderProgram(std::shared_ptr<ShaderProgram> shaderProgram);

    virtual void draw(const Camera* camera) const;
    virtual void draw(ShaderProgram* shaderProgram, const Camera* camera) const;
    virtual void update(float delta);

    TransformationComponent* transformation() { return m_tr; }
    const TransformationComponent* transformation() const { return m_tr; }

    RenderComponent* render() { return m_rd; }
    const RenderComponent* render() const { return m_rd; }

  protected:
    const glm::mat4& modelMatrix() const { return m_modelMatrix; }

    const int m_actorId;
    TransformationComponent* m_tr = nullptr;
    RenderComponent* m_rd         = nullptr;

  private:
    void rebuildModelMatrix();

    std::shared_ptr<Mesh> m_mesh;
    std::shared_ptr<ShaderProgram> m_shaderProgram;
    std::vector<std::shared_ptr<Texture>> m_textures;

    glm::mat4 m_modelMatrix;
};

#endif // RENDERNODE_H
