#include "Node.h"

#include "Light.h"
#include "Model.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>

namespace gfx {

Node::Node(int actorId, TransformationComponent* tr, RenderComponent* rd)
    : m_actorId{actorId}
    , m_tr{tr}
    , m_rd{rd}
{
}

//------------------------------------------------------------------------------

void Node::rebuildModelMatrix()
{
    if (m_tr) {
        m_rotation    = m_tr->rotation;
        m_translation = m_tr->translation;
        m_scale       = m_tr->scale;
    }

    const auto T = glm::translate(glm::mat4(1.f), m_translation);
    const auto R = glm::toMat4(m_rotation);
    const auto S = glm::scale(glm::mat4(1.f), m_scale);

    m_modelMatrix = T * R * S;
}

//------------------------------------------------------------------------------

void Node::draw(const glm::mat4& parentModelMatrix, ShaderProgram* shaderProgram,
                std::array<Light*, 8>& lights) const
{
    const auto& modelMatrix = parentModelMatrix * m_modelMatrix;

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

        shaderProgram->setUniform("modelMatrix", modelMatrix);

        // if (!m_rd->backfaceCulling) glDisable(GL_CULL_FACE);

        mesh->draw(shaderProgram);

        // if (!m_rd->backfaceCulling) glEnable(GL_CULL_FACE);
    }

    for (auto child : m_children) {
        auto n = m_model->getNode(child);
        n->draw(modelMatrix, shaderProgram, lights);
    }
}

//------------------------------------------------------------------------------

void Node::update(const glm::mat4& parentModelMatrix, float deltaTime)
{
    rebuildModelMatrix();

    const auto& worldMatrix = parentModelMatrix * m_modelMatrix;

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

void Node::drawAabb(const glm::mat4& parentModelMatrix, ShaderProgram* shaderProgram) const
{
    const auto& modelMatrix = parentModelMatrix * m_modelMatrix;

    if (m_model && m_mesh != -1) {
        auto mesh = m_model->getMesh(m_mesh);

        shaderProgram->use();

        shaderProgram->setUniform("minimum", mesh->aabb().minimum);
        shaderProgram->setUniform("maximum", mesh->aabb().maximum);

        glDrawArrays(GL_POINTS, 0, 1);
    }

    for (auto child : m_children) {
        auto n = m_model->getNode(child);
        n->drawAabb(modelMatrix, shaderProgram);
    }
}

} // namespace gfx
