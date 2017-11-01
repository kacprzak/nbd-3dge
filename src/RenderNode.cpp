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

void RenderNode::setTextures(std::vector<std::shared_ptr<Texture>> textures)
{
    m_textures = textures;
}

void RenderNode::addTexture(const std::shared_ptr<Texture>& texture)
{
    m_textures.push_back(texture);
}

void RenderNode::setMesh(const std::shared_ptr<Mesh>& mesh) { m_mesh = mesh; }

void RenderNode::setMaterials(std::vector<std::shared_ptr<Material>> materials)
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

void RenderNode::draw(const Camera* camera) const { draw(m_shaderProgram.get(), camera); }

void RenderNode::draw(ShaderProgram* shaderProgram, const Camera* camera) const
{
    if (!m_mesh) return;

    for (size_t i = 0; i < m_textures.size(); ++i) {
        m_textures[i]->bind(i);
    }

    if (shaderProgram) {
        shaderProgram->use();
        shaderProgram->setUniform("projectionMatrix", camera->projectionMatrix());
        shaderProgram->setUniform("viewMatrix", camera->viewMatrix());
        shaderProgram->setUniform("modelMatrix", m_modelMatrix);

        for (size_t i = 0; i < m_textures.size(); ++i) {
            const std::string& name = "sampler" + std::to_string(i);
            shaderProgram->setUniform(name.c_str(), int(i));
        }
    } else {
        glUseProgram(0);
    }

    if (!m_rd->backfaceCulling) glDisable(GL_CULL_FACE);

    m_mesh->draw();

    if (!m_rd->backfaceCulling) glEnable(GL_CULL_FACE);
}

void RenderNode::update(float /*deltaTime*/) { rebuildModelMatrix(); }
