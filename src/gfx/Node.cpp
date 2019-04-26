#include "Node.h"

#include "Camera.h"
#include "Light.h"
#include "Material.h"

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

void Node::setMesh(const std::shared_ptr<Mesh>& mesh) { m_mesh = mesh; }

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
                const Camera* camera, const std::array<Light*, 8>& lights,
                const TexturePack& environment) const
{
    const auto& modelMatrix = parentModelMatrix * m_modelMatrix;

    if (m_mesh) {
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

        shaderProgram->setUniform("projectionMatrix", camera->projectionMatrix());
        shaderProgram->setUniform("viewMatrix", camera->viewMatrix());
        shaderProgram->setUniform("modelMatrix", modelMatrix);

        shaderProgram->setUniform("cameraPosition", camera->worldTranslation());

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

        // if (!m_rd->backfaceCulling) glDisable(GL_CULL_FACE);

        m_mesh->draw(shaderProgram);

        // if (!m_rd->backfaceCulling) glEnable(GL_CULL_FACE);
    }

    for (auto child : m_children) {
        child->draw(modelMatrix, shaderProgram, camera, lights, environment);
    }
}

//------------------------------------------------------------------------------

void Node::update(const glm::mat4& parentModelMatrix, float deltaTime)
{
    rebuildModelMatrix();

    const auto& worldMatrix = parentModelMatrix * m_modelMatrix;

    if (m_camera) m_camera->update(worldMatrix, deltaTime);
    if (m_light) m_light->update(worldMatrix, deltaTime);

    for (auto child : m_children) {
        child->update(worldMatrix, deltaTime);
    }
}

//------------------------------------------------------------------------------

void Node::drawAabb(const glm::mat4& parentModelMatrix, ShaderProgram* shaderProgram,
                    const Camera* camera)
{
    const auto& modelMatrix = parentModelMatrix * m_modelMatrix;

    if (m_mesh) {
        shaderProgram->use();

        shaderProgram->setUniform("projectionMatrix", camera->projectionMatrix());
        shaderProgram->setUniform("viewMatrix", camera->viewMatrix());
        shaderProgram->setUniform("modelMatrix", modelMatrix);

        shaderProgram->setUniform("minimum", m_mesh->aabb().minimum);
        shaderProgram->setUniform("maximum", m_mesh->aabb().maximum);

        glDrawArrays(GL_POINTS, 0, 1);
    }

    for (auto child : m_children) {
        child->drawAabb(modelMatrix, shaderProgram, camera);
    }
}

} // namespace gfx
