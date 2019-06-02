#ifndef SKYBOX_H
#define SKYBOX_H

#include "Mesh.h"
#include "ShaderProgram.h"
#include "Texture.h"

#include <glm/glm.hpp>

namespace gfx {

class Camera;

class Skybox final
{
  public:
    Skybox();

    void setShaderProgram(const std::shared_ptr<ShaderProgram>& shaderProgram)
    {
        m_shaderProgram = shaderProgram;
    }

    void setTextures(const TexturePack& textures) { m_textures = textures; }

    TexturePack textures() { return m_textures; };

    void draw(const Camera* camera) const;

  private:
    std::shared_ptr<ShaderProgram> m_shaderProgram;
    std::shared_ptr<Mesh> m_mesh;
    TexturePack m_textures{};
};

} // namespace gfx

#endif // SKYBOX_H
