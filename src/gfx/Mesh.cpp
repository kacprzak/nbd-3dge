#include "Mesh.h"

#include "../Logger.h"

#include <array>
#include <cstring>
#include <limits>
#include <numeric> // iota
#include <queue>   // priority_queue

namespace gfx {

Primitive::Primitive(Attributes attributes, Accessor indices, GLenum mode,
                     std::vector<MorphTarget> targets)
    : m_attributes{attributes}
    , m_indices{indices}
    , m_mode{mode}
    , m_targets{targets}
    , m_activeTargets{{-1, -1, -1}}
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

    int index = 0;
    for (const Accessor& acc : m_attributes) {
        setVertexAttribute(index++, acc);
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
    std::swap(m_activeTargets, other.m_activeTargets);
    std::swap(m_activeTargetsDirty, other.m_activeTargetsDirty);
    std::swap(m_material, other.m_material);
}

//------------------------------------------------------------------------------

Primitive::~Primitive()
{
    if (m_vao) {
        glDeleteVertexArrays(1, &m_vao);
        LOG_RELEASED;
    }
}

//------------------------------------------------------------------------------

void Primitive::draw(ShaderProgram* shaderProgram)
{
    shaderProgram->use();
    m_material.applyTo(shaderProgram);

    glBindVertexArray(m_vao);

    if (m_activeTargetsDirty) {
        updateActiveTargets();
        m_activeTargetsDirty = false;
    }

    if (m_indices.count == 0) {
        glDrawArrays(m_mode, 0, m_attributes[Accessor::Attribute::Position].count);
    } else {
        glDrawElements(m_mode, m_indices.count, m_indices.type, (const void*)m_indices.byteOffset);
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

//------------------------------------------------------------------------------

void Primitive::setActiveTargets(const std::array<int, 3>& targets)
{
    if (m_activeTargets != targets) {
        m_activeTargets      = targets;
        m_activeTargetsDirty = true;
    }
}

//------------------------------------------------------------------------------

void Primitive::setVertexAttribute(int index, const Accessor& acc)
{
    if (acc.count > 0) {
        acc.buffer->bind(GL_ARRAY_BUFFER);
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(index, acc.size, acc.type, acc.normalized, acc.buffer->m_byteStride,
                              (const void*)acc.byteOffset);
    } else {
        glDisableVertexAttribArray(index);
    }
}

//------------------------------------------------------------------------------

void Primitive::updateActiveTargets()
{
    int index = Accessor::Attribute::Size;
    for (auto at : m_activeTargets) {
        if (at == -1) break;

        const MorphTarget& mt = m_targets[at];
        for (const Accessor& acc : mt) {
            setVertexAttribute(index++, acc);
        }
    }
}

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

void Mesh::draw(ShaderProgram* shaderProgram) { draw(shaderProgram, m_weights); }

//------------------------------------------------------------------------------

void Mesh::draw(ShaderProgram* shaderProgram, const std::vector<float>& weights)
{
    shaderProgram->use();

    const auto& activeTargets = selectActiveTargets(weights);

    std::array<float, 3> w{};
    std::transform(std::cbegin(activeTargets),
                   std::find(std::cbegin(activeTargets), std::cend(activeTargets), -1),
                   std::begin(w), [&weights](int i) { return weights[i]; });

    shaderProgram->setUniform("weights", glm::vec3{w[0], w[1], w[2]});

    for (auto& primitive : m_primitives) {
        primitive.setActiveTargets(activeTargets);
        primitive.draw(shaderProgram);
    }
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

//------------------------------------------------------------------------------

std::size_t Mesh::getWeightsSize() const
{
    std::size_t ans = 0u;
    for (const auto& p : m_primitives) {
        ans = std::max(ans, p.targetsSize());
    }
    return ans;
}

//------------------------------------------------------------------------------

std::array<int, 3> Mesh::selectActiveTargets(const std::vector<float>& weights) const
{
    std::array<int, 3> activeTargets = {-1, -1, -1};

    if (weights.empty()) {
        // do nothing
    } else if (weights.size() <= activeTargets.size()) {
        // just populate in default order
        auto begin = std::begin(activeTargets);
        std::iota(begin, begin + weights.size(), 0);
    } else {
        // select targets with largest weights
        const auto cmp = [&weights](int left, int right) { return weights[left] < weights[right]; };
        std::priority_queue<int, std::vector<int>, decltype(cmp)> queue{cmp};
        for (int i = 0; i < weights.size(); ++i)
            queue.push(i);

        for (auto& at : activeTargets) {
            at = queue.top();
            queue.pop();
        }
    }

    return activeTargets;
}

} // namespace gfx
