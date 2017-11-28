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
    , m_modelMatrix(glm::mat4(1.0f))
{
}

//------------------------------------------------------------------------------

void RenderNode::setMesh(const std::shared_ptr<Mesh>& mesh) { m_mesh = mesh; }

void RenderNode::setMaterial(const std::shared_ptr<Material>& material) { m_material = material; }

void RenderNode::setShaderProgram(const std::shared_ptr<ShaderProgram>& shaderProgram)
{
    m_shaderProgram = shaderProgram;
}

//------------------------------------------------------------------------------

void RenderNode::rebuildModelMatrix()
{
    const auto T = glm::translate(glm::mat4(1.f), m_tr->position);
    const auto R = glm::toMat4(m_tr->orientation);
    const auto S = glm::scale(glm::mat4(1.f), glm::vec3{m_tr->scale, m_tr->scale, m_tr->scale});

    m_modelMatrix = T * R * S;
}

//------------------------------------------------------------------------------

void RenderNode::draw(const Camera* camera, const std::array<Light*, 8>& lights,
                      Texture* environment) const
{
    draw(m_shaderProgram.get(), camera, lights, environment);
}

void RenderNode::draw(ShaderProgram* shaderProgram, const Camera* camera,
                      const std::array<Light*, 8>& lights, Texture* environment) const
{
    if (!m_mesh) return;

    if (shaderProgram) {
        shaderProgram->use();

        int textureUnit = 0;
        if (m_material) {
            for (const auto& texture : m_material->textures) {
                const std::string& name = "sampler" + std::to_string(textureUnit);
                shaderProgram->setUniform(name.c_str(), textureUnit);

                texture->bind(textureUnit++);
            }
        }

        for (size_t i = 0; i < lights.size(); ++i) {
            if (lights[i]) {
                const auto& light    = *lights[i];
                const auto& lightIdx = "lights[" + std::to_string(i) + "]";
                shaderProgram->setUniform((lightIdx + ".position").c_str(), light.position());
                shaderProgram->setUniform((lightIdx + ".ambient").c_str(), light.ambient());
                shaderProgram->setUniform((lightIdx + ".diffuse").c_str(), light.diffuse());
                shaderProgram->setUniform((lightIdx + ".specular").c_str(), light.specular());

                if (i == 0) {
                    shaderProgram->setUniform(
                        "lightMVP", light.projectionMatrix() * light.viewMatrix() * m_modelMatrix);
                }
            }
        }

        if (m_material) {
            const auto& material = *m_material;
            shaderProgram->setUniform("material.ambient", material.ambient());
            shaderProgram->setUniform("material.diffuse", material.diffuse());
            shaderProgram->setUniform("material.specular", material.specular());
            shaderProgram->setUniform("material.emission", material.emission());
            shaderProgram->setUniform("material.shininess", material.shininess());
        }

        shaderProgram->setUniform("projectionMatrix", camera->projectionMatrix());
        shaderProgram->setUniform("viewMatrix", camera->viewMatrix());
        shaderProgram->setUniform("modelMatrix", m_modelMatrix);

        if (environment) {
            shaderProgram->setUniform("cameraPosition", camera->transformation()->position);
            shaderProgram->setUniform("environmentCube", textureUnit);

            environment->bind(textureUnit++);
        }

    } else {
        glUseProgram(0);
    }

    if (!m_rd->backfaceCulling) glDisable(GL_CULL_FACE);

    m_mesh->draw();

    if (!m_rd->backfaceCulling) glEnable(GL_CULL_FACE);
}

//------------------------------------------------------------------------------

void RenderNode::update(float /*deltaTime*/) { rebuildModelMatrix(); }

//------------------------------------------------------------------------------

void RenderNode::drawAabb(ShaderProgram* shaderProgram, const Camera* camera)
{
    shaderProgram->use();

    shaderProgram->setUniform("projectionMatrix", camera->projectionMatrix());
    shaderProgram->setUniform("viewMatrix", camera->viewMatrix());
    shaderProgram->setUniform("modelMatrix", m_modelMatrix);
    shaderProgram->setUniform("minimum", m_mesh->aabb().leftBottomNear);
    shaderProgram->setUniform("maximum", m_mesh->aabb().rightTopFar);

    glDrawArrays(GL_POINTS, 0, 1);
}
