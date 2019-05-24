#ifndef MESH_H
#define MESH_H

#include "Aabb.h"
#include "Buffer.h"
#include "Macros.h"
#include "Material.h"
#include "ShaderProgram.h"

#include <GL/glew.h>

namespace gfx {

class Mesh final
{
    OSTREAM_FRIEND(Mesh);

    using Attributes = std::array<Accessor, Accessor::Attribute::Size>;

  public:
    Mesh(Attributes attributes, Accessor indices, GLenum primitive,
         std::vector<Attributes> targets = {});

    Mesh(Mesh&& other);
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    ~Mesh();

    void draw(ShaderProgram* shaderProgram) const;

    std::vector<float> positions() const;
    Aabb aabb() const;

    void setMaterial(const Material& material);

  private:
    GLuint m_vao;

    Attributes m_attributes;
    Accessor m_indices;
    GLenum m_primitive = GL_TRIANGLES;
    std::vector<Attributes> m_targets;

    Material m_material;

  public:
    std::string name;
};

OSTREAM_IMPL_1(gfx::Mesh, m_vao)

} // namespace gfx

#endif // MESH_H
