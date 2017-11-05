#include "RenderNode.h"

#include "Camera.h"

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

void RenderNode::setTextures(const std::vector<std::shared_ptr<Texture>>& textures)
{
    m_textures = textures;
}

void RenderNode::addTexture(const std::shared_ptr<Texture>& texture)
{
    m_textures.push_back(texture);
}

void RenderNode::setMesh(const std::shared_ptr<Mesh>& mesh) { m_mesh = mesh; }

void RenderNode::setMaterials(const std::vector<std::shared_ptr<Material>>& materials)
{
    m_materials = materials;
}

void RenderNode::addMaterial(const std::shared_ptr<Material>& material)
{
    m_materials.push_back(material);
}

void RenderNode::setShaderProgram(const std::shared_ptr<ShaderProgram>& shaderProgram)
{
    m_shaderProgram = shaderProgram;
}

void RenderNode::rebuildModelMatrix()
{
    const auto T = glm::translate(glm::mat4(1.f), m_tr->position);
    const auto R = glm::toMat4(m_tr->orientation);
    const auto S = glm::scale(glm::mat4(1.f), glm::vec3{m_tr->scale, m_tr->scale, m_tr->scale});

    m_modelMatrix = T * R * S;
}

void RenderNode::draw(const Camera* camera, const std::array<Light*, 8>& lights) const
{
    draw(m_shaderProgram.get(), camera, lights);
}

void RenderNode::draw(ShaderProgram* shaderProgram, const Camera* camera,
                      const std::array<Light*, 8>& lights) const
{
    if (!m_mesh) return;

    if (shaderProgram) {
        shaderProgram->use();

        int textureUnit = 0;
        for (const auto& material : m_materials) {
            for (const auto& texture : material->textures) {
                const std::string& name = "sampler" + std::to_string(textureUnit);
                shaderProgram->setUniform(name.c_str(), textureUnit);

                texture->bind(textureUnit++);
            }
        }

        for (const auto& texture : m_textures) {
            const std::string& name = "sampler" + std::to_string(textureUnit);
            shaderProgram->setUniform(name.c_str(), textureUnit);

            texture->bind(textureUnit++);
        }

        for (size_t i = 0; i < lights.size(); ++i) {
            if (lights[i]) {
                const auto& light    = *lights[i];
                const auto& lightIdx = "lights[" + std::to_string(i) + "]";
                shaderProgram->setUniform((lightIdx + ".position").c_str(), light.position());
                shaderProgram->setUniform((lightIdx + ".ambient").c_str(), light.ambient());
                shaderProgram->setUniform((lightIdx + ".diffuse").c_str(), light.diffuse());
                shaderProgram->setUniform((lightIdx + ".specular").c_str(), light.specular());
            }
        }

        for (size_t i = 0; i < m_materials.size(); ++i) {
            const auto& material = *m_materials[i];
            shaderProgram->setUniform("material.ambient", material.ambient());
            shaderProgram->setUniform("material.diffuse", material.diffuse());
            shaderProgram->setUniform("material.specular", material.specular());
            shaderProgram->setUniform("material.emission", material.emission());
            shaderProgram->setUniform("material.shininess", material.shininess());
        }

        shaderProgram->setUniform("projectionMatrix", camera->projectionMatrix());
        shaderProgram->setUniform("viewMatrix", camera->viewMatrix());
        shaderProgram->setUniform("modelMatrix", m_modelMatrix);

    } else {
        glUseProgram(0);
    }

    if (!m_rd->backfaceCulling) glDisable(GL_CULL_FACE);

    m_mesh->draw();

    if (!m_rd->backfaceCulling) glEnable(GL_CULL_FACE);
}

void RenderNode::update(float /*deltaTime*/) { rebuildModelMatrix(); }
