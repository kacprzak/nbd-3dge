#include "Model.h"

namespace gfx {

void Model::draw(const glm::mat4& transformation, ShaderProgram* shaderProgram,
                 std::array<Light*, 8>& lights, const TexturePack& environment)
{
    shaderProgram->use();

    if (auto& t = environment[TextureUnit::BrdfLUT]) {
        shaderProgram->setUniform("brdfLUT", TextureUnit::BrdfLUT);
        t->bind(TextureUnit::BrdfLUT);
    }
    if (auto& t = environment[TextureUnit::Irradiance]) {
        shaderProgram->setUniform("irradianceCube", TextureUnit::Irradiance);
        t->bind(TextureUnit::Irradiance);
    }
    if (auto& t = environment[TextureUnit::Radiance]) {
        shaderProgram->setUniform("radianceCube", TextureUnit::Radiance);
        t->bind(TextureUnit::Radiance);
    }

    for (const auto& scene : m_scenes)
        for (auto rootIdx : scene)
            getNode(rootIdx)->draw(transformation, shaderProgram, lights);
}

void Model::drawAabb(const glm::mat4& transformation, ShaderProgram* shaderProgram)
{
    for (const auto& scene : m_scenes)
        for (auto rootIdx : scene)
            getNode(rootIdx)->drawAabb(transformation, shaderProgram);
}

void Model::update(float delta)
{
    glm::mat4 identity{1.0f};

    for (auto& animation : m_animations)
        animation.update(delta, m_nodes);

    for (const auto& scene : m_scenes)
        for (auto rootIdx : scene)
            getNode(rootIdx)->update(identity, delta);
}

Node* Model::findNode(const std::string& node)
{
    for (auto& n : m_nodes) {
        if (n.name == node) return &n;
    }
    return nullptr;
}

Aabb Model::aabb(const glm::mat4& transformation) const
{
    Aabb aabb;
    for (const auto& scene : m_scenes)
        for (auto rootIdx : scene)
            aabb = aabb.mbr(getNode(rootIdx)->aabb(transformation));

    return aabb;
}

int Model::addNode(Node node, Node* parent)
{
    if (parent->getModel() != this) throw std::invalid_argument{"parent not part of model"};

    m_nodes.push_back(node);
    int idx = m_nodes.size() - 1;

    parent->addChild(idx);

    return idx;
}

Node* Model::getNode(int idx)
{
    if (idx >= 0 || idx < m_nodes.size()) return &m_nodes[idx];
    return nullptr;
}

const Node* Model::getNode(int idx) const
{
    if (idx >= 0 || idx < m_nodes.size()) return &m_nodes[idx];
    return nullptr;
}

} // namespace gfx
