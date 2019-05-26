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

    using Attributes  = std::array<Accessor, Accessor::Attribute::Size>;
    using MorphTarget = std::array<Accessor, 3>; // positions, normals, tangents

  public:
    Mesh(Attributes attributes, Accessor indices, GLenum primitive,
         std::vector<MorphTarget> targets = {});

    Mesh(Mesh&& other);
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    ~Mesh();

    void draw(ShaderProgram* shaderProgram) const;
    void draw(ShaderProgram* shaderProgram, const std::vector<float>& weights) const;

    std::vector<float> positions() const;
    Aabb aabb() const;

    void setMaterial(const Material& material);

    void setWeights(const std::vector<float>& weights);
    std::size_t morphTargetsSize() const { return m_targets.size(); }

  private:
    GLuint m_vao;

    Attributes m_attributes;
    Accessor m_indices;
    GLenum m_primitive = GL_TRIANGLES;

    Material m_material;

    std::vector<MorphTarget> m_targets;
    std::vector<float> m_weights; //< Default weights

  public:
    std::string name;
};

OSTREAM_IMPL_1(gfx::Mesh, m_vao)

} // namespace gfx

#endif // MESH_H
