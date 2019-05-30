#include "Node.h"

#include "Light.h"
#include "Model.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>

namespace gfx {

Node::Node() {}

//------------------------------------------------------------------------------

std::size_t Node::getWeightsSize() const
{
    if (m_model && m_mesh != -1) {
        auto mesh = m_model->getMesh(m_mesh);
        return mesh->getWeightsSize();
    }
    return 0;
}

//------------------------------------------------------------------------------

void Node::rebuildModelMatrix()
{
    const auto T = glm::translate(glm::mat4(1.f), m_translation);
    const auto R = glm::toMat4(m_rotation);
    const auto S = glm::scale(glm::mat4(1.f), m_scale);

    m_modelMatrix = T * R * S;
}

//------------------------------------------------------------------------------

void Node::draw(const glm::mat4& transformation, ShaderProgram* shaderProgram,
                std::array<Light*, 8>& lights) const
{
    const auto& worldMatrix = transformation * m_modelMatrix;

    if (m_model && m_mesh != -1) {
        auto mesh = m_model->getMesh(m_mesh);

        shaderProgram->use();

        for (size_t i = 0; i < lights.size(); ++i) {
            if (lights[i]) {
                const auto& light = *lights[i];
                light.applyTo(shaderProgram, i);

                const auto& lightMVPIndex = "lightMVP[" + std::to_string(i) + "]";
                shaderProgram->setUniform(lightMVPIndex.c_str(), light.projectionMatrix() *
                                                                     light.viewMatrix() *
                                                                     m_modelMatrix);
            }
        }

        shaderProgram->setUniform("modelMatrix", worldMatrix);

        if (!m_weights.empty())
            mesh->draw(shaderProgram, m_weights);
        else
            mesh->draw(shaderProgram); // draw with default weights
    }

    for (auto child : m_children) {
        auto n = m_model->getNode(child);
        n->draw(worldMatrix, shaderProgram, lights);
    }
}

//------------------------------------------------------------------------------

void Node::update(const glm::mat4& transformation, float deltaTime)
{
    rebuildModelMatrix();

    const auto& worldMatrix = transformation * m_modelMatrix;

    if (m_model) {
        if (auto camera = m_model->getCamera(m_camera)) camera->update(worldMatrix, deltaTime);
        if (auto light = m_model->getLight(m_light)) light->update(worldMatrix, deltaTime);
    }

    for (auto child : m_children) {
        auto n = m_model->getNode(child);
        n->update(worldMatrix, deltaTime);
    }
}

//------------------------------------------------------------------------------

Aabb Node::aabb() const
{
    Aabb aabb;

    if (m_model && m_mesh != -1) aabb = aabb.mbr(m_modelMatrix * m_model->getMesh(m_mesh)->aabb());

    for (auto& child : m_children) {
        auto n = m_model->getNode(child);
        aabb   = aabb.mbr(m_modelMatrix * n->aabb());
    }
    return aabb;
}

//------------------------------------------------------------------------------

void Node::drawAabb(const glm::mat4& transformation, ShaderProgram* shaderProgram) const
{
    const auto& worldMatrix = transformation * m_modelMatrix;

    if (m_model && m_mesh != -1) {
        auto mesh = m_model->getMesh(m_mesh);

        shaderProgram->use();

        shaderProgram->setUniform("modelMatrix", worldMatrix);
        shaderProgram->setUniform("minimum", mesh->aabb().minimum);
        shaderProgram->setUniform("maximum", mesh->aabb().maximum);

        glDrawArrays(GL_POINTS, 0, 1);
    }

    for (auto child : m_children) {
        auto n = m_model->getNode(child);
        n->drawAabb(worldMatrix, shaderProgram);
    }
}

} // namespace gfx
