#ifndef MESH_H
#define MESH_H

#include "Aabb.h"
#include "Buffer.h"
#include "Material.h"
#include "ShaderProgram.h"

#include <GL/glew.h>

class Mesh final
{
  public:
    Mesh(std::array<Accessor, Accessor::Attribute::Size> attributes, Accessor indices,
         GLenum primitive);

    Mesh(Mesh&& other);
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    ~Mesh();

    void draw(ShaderProgram* shaderProgram) const;

    std::vector<float> positions() const;
    Aabb aabb() const { return m_aabb; }

    void setMaterial(const std::shared_ptr<Material>& material);

  protected:
    const Material* getMaterial() const;

  private:
    GLuint m_vao;

    GLenum m_primitive              = GL_TRIANGLES;
    GLenum m_typeOfElement          = GL_UNSIGNED_SHORT;
    unsigned int m_numberOfElements = 0u;
    unsigned int m_numberOfVertices = 0u;

    Aabb m_aabb;

    Accessor m_positionsAcc;
    std::shared_ptr<Material> m_material;

  public:
    std::string name;
};

#endif // MESH_H
