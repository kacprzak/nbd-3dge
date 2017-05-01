#ifndef GFXNODE_H
#define GFXNODE_H

#include "Mesh.h"
#include "Texture.h"
#include "ShaderProgram.h"
#include "Components.h"

#include <glm/glm.hpp>

class Camera;

class GfxNode
{
 public:
    GfxNode(int actorId, TransformationComponent* tr);
    virtual ~GfxNode() {};

    void setTextures(std::vector<std::shared_ptr<Texture>> textures);
    void addTexture(std::shared_ptr<Texture> texture);
    
    void setMesh(std::shared_ptr<Mesh> mesh);
    void setShaderProgram(std::shared_ptr<ShaderProgram> shaderProgram);

    virtual void draw(const Camera* camera) const;
    virtual void draw(ShaderProgram* shaderProgram, const Camera* camera) const;
    virtual void update(float delta);

 protected:
    const glm::mat4& modelMatrix() const { return m_modelMatrix; }

    const int m_actorId;
    TransformationComponent* m_tr;

 private:
    void rebuildModelMatrix();
    
    std::shared_ptr<Mesh> m_mesh;
    std::shared_ptr<ShaderProgram> m_shaderProgram;
    std::vector<std::shared_ptr<Texture>> m_textures;
    
    glm::mat4 m_modelMatrix;
};

#endif // GFXNODE_H
