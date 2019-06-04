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

void Node::setTranslation(glm::vec3 translation)
{
    m_translation      = translation;
    m_modelMatrixDirty = true;
}

//------------------------------------------------------------------------------

void Node::setRotation(glm::quat rotation)
{
    m_rotation         = rotation;
    m_modelMatrixDirty = true;
}

//------------------------------------------------------------------------------

void Node::setScale(glm::vec3 scale)
{
    m_scale            = scale;
    m_modelMatrixDirty = true;
}

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

void Node::setModelMatrix(const glm::mat4& mtx)
{
    m_modelMatrix = mtx;

    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(mtx, m_scale, m_rotation, m_translation, skew, perspective);
}

//------------------------------------------------------------------------------

void Node::draw(const glm::mat4& transformation, ShaderProgram* shaderProgram,
                std::array<Light*, 8>& lights) const
{
    const auto& worldMatrix  = transformation * m_modelMatrix;
    const auto& normalMatrix = glm::transpose(glm::inverse(glm::mat3(worldMatrix)));

    if (m_model && m_mesh != -1) {
        auto mesh = m_model->getMesh(m_mesh);

        shaderProgram->use();

        for (size_t i = 0; i < lights.size(); ++i) {
            if (lights[i]) {
                const auto& light = *lights[i];
                light.applyTo(shaderProgram, i);

                const auto& lightMVPIndex = "lightMVP[" + std::to_string(i) + "]";
                shaderProgram->setUniform(lightMVPIndex, light.projectionMatrix() *
                                                             light.viewMatrix() * m_modelMatrix);
            }
        }

        std::array<glm::mat4, 20> jointMatrices{};
        for (auto& m : jointMatrices)
            m = glm::mat4{1.0f};

        if (m_skin != -1) {
            const Skin* skin = m_model->getSkin(m_skin);
            skin->calculateJointMatrices(jointMatrices, this);
        }

        for (int i = 0; i < jointMatrices.size(); ++i) {
            const auto& jointMatIndex = "jointMatrices[" + std::to_string(i) + "]";
            shaderProgram->setUniform(jointMatIndex, jointMatrices[i]);
        }

        shaderProgram->setUniform("modelViewMatrix", worldMatrix);
        shaderProgram->setUniform("normalMatrix", normalMatrix);

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
    if (m_modelMatrixDirty) {
        rebuildModelMatrix();
        m_modelMatrixDirty = false;
    }

    m_worldMatrix = transformation * m_modelMatrix;

    if (auto camera = m_model->getCamera(m_camera)) camera->update(m_worldMatrix, deltaTime);
    if (auto light = m_model->getLight(m_light)) light->update(m_worldMatrix, deltaTime);

    for (auto child : m_children) {
        auto n = m_model->getNode(child);
        n->update(m_worldMatrix, deltaTime);
    }
}

//------------------------------------------------------------------------------

Aabb Node::aabb(const glm::mat4& transformation) const
{
    Aabb aabb;
    const auto& tm = transformation * m_modelMatrix;

    if (m_model && m_mesh != -1) aabb = aabb.mbr(m_model->getMesh(m_mesh)->aabb(tm));

    for (auto& child : m_children) {
        auto n = m_model->getNode(child);
        aabb   = aabb.mbr(n->aabb(tm));
    }
    return aabb;
}

//------------------------------------------------------------------------------

void Node::drawAabb(const glm::mat4& transformation, ShaderProgram* shaderProgram) const
{
    const auto& worldMatrix  = transformation * m_modelMatrix;
    const auto& normalMatrix = glm::transpose(glm::inverse(glm::mat3(worldMatrix)));

    if (m_model && m_mesh != -1) {
        auto mesh = m_model->getMesh(m_mesh);

        shaderProgram->use();

        const auto& box = mesh->aabb(glm::mat4{1.0f});

        shaderProgram->setUniform("modelViewMatrix", worldMatrix);
        shaderProgram->setUniform("normalMatrix", normalMatrix);
        shaderProgram->setUniform("minimum", box.minimum);
        shaderProgram->setUniform("maximum", box.maximum);

        glDrawArrays(GL_POINTS, 0, 1);
    }

    for (auto child : m_children) {
        auto n = m_model->getNode(child);
        n->drawAabb(worldMatrix, shaderProgram);
    }
}

} // namespace gfx
