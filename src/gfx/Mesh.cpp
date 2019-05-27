#include "Mesh.h"

#include "../Logger.h"

#include <array>
#include <cstring>
#include <limits>

namespace gfx {

Primitive::Primitive(Attributes attributes, Accessor indices, GLenum mode,
                     std::vector<MorphTarget> targets)
    : m_attributes{attributes}
    , m_indices{indices}
    , m_mode{mode}
    , m_targets{targets}
{
    if (m_attributes[Accessor::Attribute::Position].count == 0) {
        auto msg = "Primitive must have positions buffer";
        LOG_ERROR(msg);
        throw std::runtime_error{msg};
    }

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    if (m_indices.count > 0) {
        m_indices.buffer->bind(GL_ELEMENT_ARRAY_BUFFER);
    }

    const auto defineArray = [](int index, const Accessor& acc) {
        if (acc.count > 0) {
            acc.buffer->bind(GL_ARRAY_BUFFER);
            glEnableVertexAttribArray(index);
            glVertexAttribPointer(index, acc.size, acc.type, acc.normalized,
                                  acc.buffer->m_byteStride, (const void*)acc.byteOffset);
        }
    };

    int index = 0;
    for (const Accessor& acc : m_attributes) {
        defineArray(index++, acc);
    }

    for (const MorphTarget& mt : m_targets) {
        for (const Accessor& acc : mt) {
            defineArray(index++, acc);
        }
    }

    LOG_CREATED;

    glBindVertexArray(0);
}

//------------------------------------------------------------------------------

Primitive::Primitive(Primitive&& other)
{
    std::swap(m_vao, other.m_vao);
    std::swap(m_attributes, other.m_attributes);
    std::swap(m_indices, other.m_indices);
    std::swap(m_mode, other.m_mode);
    std::swap(m_targets, other.m_targets);
}

//------------------------------------------------------------------------------

Primitive::~Primitive()
{
    glDeleteVertexArrays(1, &m_vao);

    LOG_RELEASED;
}

//------------------------------------------------------------------------------

void Primitive::draw(ShaderProgram* shaderProgram, const std::vector<float>& weights) const
{
    if (shaderProgram) {
        shaderProgram->use();
        m_material.applyTo(shaderProgram);

        glm::vec3 wgh{};
        for (auto i = 0u; i < weights.size() && i < wgh.length(); ++i)
            wgh[i] = weights[i];
        shaderProgram->setUniform("weights", wgh);
    }

    glBindVertexArray(m_vao);

    if (m_indices.count == 0) {
        glDrawArrays(m_mode, 0, m_attributes[Accessor::Attribute::Position].count);
    } else {
        glDrawElements(m_mode, m_indices.count, m_indices.type, 0);
    }

    glBindVertexArray(0);
}

//------------------------------------------------------------------------------

std::vector<glm::vec3> Primitive::positions() const
{
    return m_attributes[Accessor::Attribute::Position].getData<glm::vec3>();
}

//------------------------------------------------------------------------------

Aabb Primitive::aabb() const
{
    auto minPos = m_attributes[Accessor::Attribute::Position].min;
    auto maxPos = m_attributes[Accessor::Attribute::Position].max;

    return {{minPos[0], minPos[1], minPos[2]}, {maxPos[0], maxPos[1], maxPos[2]}};
}

//------------------------------------------------------------------------------

void Primitive::setMaterial(const Material& material) { m_material = material; }

//==============================================================================

Mesh::Mesh(std::vector<Primitive>&& primitives)
    : m_primitives{std::move(primitives)}
{
}

//------------------------------------------------------------------------------

Mesh::Mesh(Primitive::Attributes attributes, Accessor indices, GLenum mode,
           std::vector<Primitive::MorphTarget> targets)
{
    m_primitives.emplace_back(attributes, indices, mode, targets);
}

//------------------------------------------------------------------------------

Mesh::Mesh(Mesh&& other)
{
    std::swap(m_primitives, other.m_primitives);
    std::swap(m_weights, other.m_weights);
}

//------------------------------------------------------------------------------

void Mesh::draw(ShaderProgram* shaderProgram) const { draw(shaderProgram, m_weights); }

//------------------------------------------------------------------------------

void Mesh::draw(ShaderProgram* shaderProgram, const std::vector<float>& weights) const
{
    for (auto& primitive : m_primitives)
        primitive.draw(shaderProgram, weights);
}

//------------------------------------------------------------------------------

std::vector<glm::vec3> Mesh::positions() const
{
    std::vector<glm::vec3> p;
    for (const auto& primitive : m_primitives) {
        const auto& tmp = primitive.positions();
        p.insert(std::end(p), std::begin(tmp), std::end(tmp));
    }
    return p;
}

//------------------------------------------------------------------------------

Aabb Mesh::aabb() const
{
    Aabb aabb;
    for (const auto& primitive : m_primitives)
        aabb = aabb.mbr(primitive.aabb());
    return aabb;
}

//------------------------------------------------------------------------------

void Mesh::setWeights(const std::vector<float>& weights) { m_weights = weights; }

} // namespace gfx
