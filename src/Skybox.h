#ifndef SKYBOX_H
#define SKYBOX_H

#include "Texture.h"
#include "Mesh.h"
#include "ShaderProgram.h"
#include <glm/glm.hpp>

class Camera;

class Skybox
{
 public:
    Skybox(std::shared_ptr<Texture> front,
           std::shared_ptr<Texture> right,
           std::shared_ptr<Texture> back,
           std::shared_ptr<Texture> left,
           std::shared_ptr<Texture> top,
           std::shared_ptr<Texture> bottom = std::shared_ptr<Texture>{});

    void setShaderProgram(std::shared_ptr<ShaderProgram> shaderProgram)
    {
        m_shaderProgram = shaderProgram;
    }
    
    void draw(const Camera* camera) const;

    void moveTo(float x, float y = 0.0f, float z = 0.0f);
    void moveTo(const glm::vec3& pos);

 private:
    static std::vector<float> quadsToTriangles3(const std::vector<float>& vertices);
    static std::vector<float> quadsToTriangles2(const std::vector<float>& vertices);

    std::shared_ptr<ShaderProgram> m_shaderProgram;
    std::vector<std::shared_ptr<Texture>> m_textures;

    glm::mat4 m_modelMatrix;

    std::shared_ptr<Mesh> m_mesh;
};

#endif // SKYBOX_H
