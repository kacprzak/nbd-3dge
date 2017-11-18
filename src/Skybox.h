#ifndef SKYBOX_H
#define SKYBOX_H

#include "Material.h"
#include "Mesh.h"
#include "ShaderProgram.h"

#include <glm/glm.hpp>

class Camera;

class Skybox final
{
  public:
    Skybox(const std::shared_ptr<Material>& material);

    void setShaderProgram(std::shared_ptr<ShaderProgram> shaderProgram)
    {
        m_shaderProgram = shaderProgram;
    }

    void draw(const Camera* camera) const;

    Material* material() { return m_material.get(); }

  private:
    std::shared_ptr<ShaderProgram> m_shaderProgram;
    std::shared_ptr<Material> m_material;
    std::shared_ptr<Mesh> m_mesh;
};

#endif // SKYBOX_H
