#ifndef MESH_H
#define MESH_H

#include "Aabb.h"
#include "Buffer.h"
#include "Macros.h"
#include "Material.h"
#include "ShaderProgram.h"

#include <GL/glew.h>

namespace gfx {

class Primitive final
{
    OSTREAM_FRIEND(Primitive);

  public:
    using Attributes  = std::array<Accessor, Accessor::Attribute::Size>;
    using MorphTarget = std::array<Accessor, 3>; // positions, normals, tangents

    Primitive(Attributes attributes, Accessor indices, GLenum mode,
              std::vector<MorphTarget> targets = {});

    Primitive(Primitive&& other);
    Primitive(const Primitive&) = delete;
    Primitive& operator=(const Primitive&) = delete;

    ~Primitive();

    void draw(ShaderProgram* shaderProgram, const std::vector<float>& weights) const;

    std::vector<glm::vec3> positions() const;
    Aabb aabb() const;

    void setMaterial(const Material& material);

    std::size_t morphTargetsSize() const { return m_targets.size(); }

  private:
    GLuint m_vao;

    Attributes m_attributes;
    Accessor m_indices;
    GLenum m_mode = GL_TRIANGLES;

    Material m_material;

    std::vector<MorphTarget> m_targets;
};

OSTREAM_IMPL_1(gfx::Primitive, m_vao)

//------------------------------------------------------------------------------

class Mesh final
{
  public:
    explicit Mesh(std::vector<Primitive>&& primitives);

    /// Creates Mesh with one primitive
    Mesh(Primitive::Attributes attributes, Accessor indices, GLenum mode,
         std::vector<Primitive::MorphTarget> targets = {});

    Mesh(Mesh&& other);
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    void draw(ShaderProgram* shaderProgram) const;
    void draw(ShaderProgram* shaderProgram, const std::vector<float>& weights) const;

    std::vector<glm::vec3> positions() const;
    Aabb aabb() const;

    void setWeights(const std::vector<float>& weights);

  private:
    std::vector<Primitive> m_primitives;
    std::vector<float> m_weights; //< Default weights

  public:
    std::string name;
};

} // namespace gfx

#endif // MESH_H
