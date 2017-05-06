#include "GfxNode.h"

#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>

GfxNode::GfxNode(int actorId, TransformationComponent* tr, RenderComponent* rd)
    : m_actorId{actorId}
    , m_tr{tr}
    , m_rd{rd}
    , m_modelMatrix(glm::mat4(1.0f))
{
}

void GfxNode::setTextures(std::vector<std::shared_ptr<Texture>> textures) { m_textures = textures; }

void GfxNode::addTexture(std::shared_ptr<Texture> texture) { m_textures.push_back(texture); }

void GfxNode::setMesh(std::shared_ptr<Mesh> mesh) { m_mesh = mesh; }

void GfxNode::setShaderProgram(std::shared_ptr<ShaderProgram> shaderProgram)
{
    m_shaderProgram = shaderProgram;
}

void GfxNode::rebuildModelMatrix()
{
    const auto T = glm::translate(glm::mat4(1.f), m_tr->position);
    const auto R = glm::toMat4(m_tr->orientation);
    const auto S = glm::scale(glm::mat4(1.f), glm::vec3{m_tr->scale, m_tr->scale, m_tr->scale});

    m_modelMatrix = T * R * S;
}

void GfxNode::draw(const Camera* camera) const { draw(m_shaderProgram.get(), camera); }

void GfxNode::draw(ShaderProgram* shaderProgram, const Camera* camera) const
{
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

    if (m_mesh)
        m_mesh->draw();
}

void GfxNode::update(float /*deltaTime*/) { rebuildModelMatrix(); }
