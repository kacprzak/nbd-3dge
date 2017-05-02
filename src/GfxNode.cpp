#include "GfxNode.h"

#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

GfxNode::GfxNode(int actorId, TransformationComponent* tr, RenderComponent* rd)
    : m_actorId{actorId}
    , m_tr{tr}
    , m_rd{rd}
    , m_modelMatrix(glm::mat4(1.0f))
{
}

void GfxNode::setTextures(std::vector<std::shared_ptr<Texture>> textures)
{
    m_textures = textures;
}

void GfxNode::addTexture(std::shared_ptr<Texture> texture)
{
    m_textures.push_back(texture);
}

void GfxNode::setMesh(std::shared_ptr<Mesh> mesh)
{
    m_mesh = mesh;
}

void GfxNode::setShaderProgram(std::shared_ptr<ShaderProgram> shaderProgram)
{
    m_shaderProgram = shaderProgram;
}

void GfxNode::rebuildModelMatrix()
{
    m_modelMatrix = glm::mat4(1.0f);

    m_modelMatrix = glm::translate(m_modelMatrix, m_tr->position);

    m_modelMatrix = glm::rotate(m_modelMatrix, m_tr->orientation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    m_modelMatrix = glm::rotate(m_modelMatrix, m_tr->orientation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    m_modelMatrix = glm::rotate(m_modelMatrix, m_tr->orientation.z, glm::vec3(0.0f, 0.0f, 1.0f));

    glm::vec3 scale{m_tr->scale, m_tr->scale, m_tr->scale};
    m_modelMatrix = glm::scale(m_modelMatrix, scale);
}

void GfxNode::draw(const Camera* camera) const
{        
    draw(m_shaderProgram.get(), camera);
}

void GfxNode::draw(ShaderProgram* shaderProgram, const Camera* camera) const
{
    for (size_t i = 0; i < m_textures.size(); ++i) {
        m_textures[i]->bind(i);
    }

    if(shaderProgram) {
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

void GfxNode::update(float /*deltaTime*/)
{
    rebuildModelMatrix();
}
