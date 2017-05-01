#include "GfxNode.h"

#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

GfxNode::GfxNode(const std::string& name)
    : m_name{name}
    , m_state{Idle}
    , m_position(glm::vec3(0.0f))
    , m_orientation(glm::vec3(0.0f))
    , m_scale(glm::vec3(1.0f))
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

    m_modelMatrix = glm::translate(m_modelMatrix, m_position);

    m_modelMatrix = glm::rotate(m_modelMatrix, m_orientation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    m_modelMatrix = glm::rotate(m_modelMatrix, m_orientation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    m_modelMatrix = glm::rotate(m_modelMatrix, m_orientation.z, glm::vec3(0.0f, 0.0f, 1.0f));

    m_modelMatrix = glm::scale(m_modelMatrix, m_scale);
}

void GfxNode::move(float x, float y, float z)
{
    m_position += glm::vec3(x, y, z);
    m_dirty = true;
}

void GfxNode::move(const glm::vec3& pos)
{
    m_position += pos;
    m_dirty = true;
}

void GfxNode::moveTo(float x, float y, float z)
{
    m_position = glm::vec3(x, y, z);
    m_dirty = true;
}

void GfxNode::moveTo(const glm::vec3& pos)
{
    m_position = pos;
    m_dirty = true;
}

void GfxNode::moveForward(float distance)
{
    glm::vec3 base(0.0f, 0.0f, 1.0f);

    base = glm::rotateX(base, m_orientation.x);
    base = glm::rotateY(base, m_orientation.y);
    base = glm::rotateZ(base, m_orientation.z);

    move(base * distance);
}

void GfxNode::moveRight(float distance)
{
    glm::vec3 base(-1.0f, 0.0f, 0.0f);

    base = glm::rotateX(base, m_orientation.x);
    base = glm::rotateY(base, m_orientation.y);
    base = glm::rotateZ(base, m_orientation.z);

    move(base * distance);
}

void GfxNode::moveLeft(float distance)
{
    moveRight(-distance);
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
    if (m_dirty) {
        refresh();
        m_dirty = false;
    }
}

void GfxNode::refresh()
{
    rebuildModelMatrix();
}

void GfxNode::setOrientation(float x, float y, float z)
{
    m_orientation = glm::vec3(x, y, z);
    m_dirty = true;
}

void GfxNode::rotate(float x, float y, float z)
{
    m_orientation += glm::vec3(x, y, z);
    m_dirty = true;
}

void GfxNode::setScale(float s)
{
    m_scale = glm::vec3(s);
    m_dirty = true;
}
