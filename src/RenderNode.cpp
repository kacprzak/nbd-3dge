#include "RenderNode.h"

#include "Camera.h"
#include "Light.h"
#include "Material.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>

RenderNode::RenderNode(int actorId, TransformationComponent* tr, RenderComponent* rd)
    : m_actorId{actorId}
    , m_tr{tr}
    , m_rd{rd}
{
}

//------------------------------------------------------------------------------

void RenderNode::setMesh(const std::shared_ptr<Mesh>& mesh) { m_mesh = mesh; }

void RenderNode::setShaderProgram(const std::shared_ptr<ShaderProgram>& shaderProgram)
{
    m_shaderProgram = shaderProgram;
}

//------------------------------------------------------------------------------

void RenderNode::rebuildModelMatrix()
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

void RenderNode::draw(const glm::mat4& parentModelMatrix, const Camera* camera,
                      const std::array<Light*, 8>& lights, Texture* environment) const
{
    draw(parentModelMatrix, m_shaderProgram.get(), camera, lights, environment);
}

void RenderNode::draw(const glm::mat4& parentModelMatrix, ShaderProgram* shaderProgram,
                      const Camera* camera, const std::array<Light*, 8>& lights,
                      Texture* environment) const
{
    const auto& modelMatrix = parentModelMatrix * m_modelMatrix;

    if (m_mesh) {
        shaderProgram->use();

        for (size_t i = 0; i < lights.size(); ++i) {
            if (lights[i]) {
                const auto& light    = *lights[i];
                const auto& lightIdx = "lights[" + std::to_string(i) + "]";
                shaderProgram->setUniform((lightIdx + ".position").c_str(), light.position());
                shaderProgram->setUniform((lightIdx + ".ambient").c_str(), light.ambient());
                shaderProgram->setUniform((lightIdx + ".diffuse").c_str(), light.diffuse());
                shaderProgram->setUniform((lightIdx + ".specular").c_str(), light.specular());

                const auto& lightMVPIndex = "lightMVP[" + std::to_string(i) + "]";
                shaderProgram->setUniform(lightMVPIndex.c_str(), light.projectionMatrix() *
                                                                     light.viewMatrix() *
                                                                     m_modelMatrix);
            }
        }

        shaderProgram->setUniform("projectionMatrix", camera->projectionMatrix());
        shaderProgram->setUniform("viewMatrix", camera->viewMatrix());
        shaderProgram->setUniform("modelMatrix", modelMatrix);

        if (environment) {
            shaderProgram->setUniform("cameraPosition", camera->worldTranslation());
            shaderProgram->setUniform("environmentCube", 7);

            environment->bind(7);
        }

        //if (!m_rd->backfaceCulling) glDisable(GL_CULL_FACE);

        m_mesh->draw(shaderProgram);

        //if (!m_rd->backfaceCulling) glEnable(GL_CULL_FACE);
    }

    for (auto child : m_children) {
        child->draw(modelMatrix, shaderProgram, camera, lights, environment);
    }
}

//------------------------------------------------------------------------------

void RenderNode::update(const glm::mat4& parentModelMatrix, float deltaTime)
{
    rebuildModelMatrix();

    const auto& worldMatrix =  parentModelMatrix * m_modelMatrix;

    if (m_camera) m_camera->update(worldMatrix, deltaTime);

    for (auto child : m_children) {
        child->update(worldMatrix, deltaTime);
    }
}

//------------------------------------------------------------------------------

void RenderNode::drawAabb(const glm::mat4& parentModelMatrix, ShaderProgram* shaderProgram,
                          const Camera* camera)
{
    shaderProgram->use();

    shaderProgram->setUniform("projectionMatrix", camera->projectionMatrix());
    shaderProgram->setUniform("viewMatrix", camera->viewMatrix());
    shaderProgram->setUniform("modelMatrix", parentModelMatrix * m_modelMatrix);

    shaderProgram->setUniform("minimum", m_mesh->aabb().minimum);
    shaderProgram->setUniform("maximum", m_mesh->aabb().maximum);

    glDrawArrays(GL_POINTS, 0, 1);
}
