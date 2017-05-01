#ifndef SKYBOX_H
#define SKYBOX_H

#include "Texture.h"
#include "Mesh.h"
#include "ShaderProgram.h"
#include <glm/glm.hpp>

class Camera;

class Skybox final
{
 public:
    Skybox(std::shared_ptr<Texture> texture);

    void setShaderProgram(std::shared_ptr<ShaderProgram> shaderProgram)
    {
        m_shaderProgram = shaderProgram;
    }
    
    void draw(const Camera* camera) const;

 private:
    std::shared_ptr<ShaderProgram> m_shaderProgram;
    std::shared_ptr<Texture> m_texture;
    std::shared_ptr<Mesh> m_mesh;
};

#endif // SKYBOX_H
