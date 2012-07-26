#ifndef SKYBOX_H
#define SKYBOX_H

#include "texture.h"
#include "mesh.h"
#include "shaderprogram.h"
#include <glm/glm.hpp>

class Skybox
{
public:
    Skybox(TexturePtr tex);

    void draw() const;
    void draw(ShaderProgram *program) const;

    void moveTo(float x, float y = 0.0f, float z = 0.0f);
    void moveTo(const glm::vec3& pos);

private:
    static std::vector<float> quadsToTriangles3(const std::vector<float>& vertices);
    static std::vector<float> quadsToTriangles2(const std::vector<float>& vertices);

    TexturePtr m_texture;
    MeshPtr m_mesh;

    glm::mat4 m_modelMatrix;
};

#endif // SKYBOX_H
