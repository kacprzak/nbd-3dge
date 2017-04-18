#include "Actor.h"

#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

Actor::Actor(const std::string& name)
    : m_name{name}
    , m_state{Idle}
    , m_position(glm::vec3(0.0f))
    , m_orientation(glm::vec3(0.0f))
    , m_scale(glm::vec3(1.0f))
    , m_modelMatrix(glm::mat4(1.0f))
{
}

void Actor::setTexture(std::shared_ptr<Texture> tex)
{
    m_texture = tex;
}

void Actor::setMesh(std::shared_ptr<Mesh> mesh)
{
    m_mesh = mesh;
}

void Actor::setShaderProgram(std::shared_ptr<ShaderProgram> shaderProgram)
{
    m_shaderProgram = shaderProgram;
}

void Actor::setScript(std::shared_ptr<Script> script)
{
    m_script = script;
}

void Actor::rebuildModelMatrix()
{
    m_modelMatrix = glm::mat4(1.0f);

    m_modelMatrix = glm::translate(m_modelMatrix, m_position);

    m_modelMatrix = glm::rotate(m_modelMatrix, m_orientation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    m_modelMatrix = glm::rotate(m_modelMatrix, m_orientation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    m_modelMatrix = glm::rotate(m_modelMatrix, m_orientation.z, glm::vec3(0.0f, 0.0f, 1.0f));

    m_modelMatrix = glm::scale(m_modelMatrix, m_scale);
}

void Actor::move(float x, float y, float z)
{
    m_position += glm::vec3(x, y, z);
    m_dirty = true;
}

void Actor::move(const glm::vec3& pos)
{
    m_position += pos;
    m_dirty = true;
}

void Actor::moveTo(float x, float y, float z)
{
    m_position = glm::vec3(x, y, z);
    m_dirty = true;
}

void Actor::moveTo(const glm::vec3& pos)
{
    m_position = pos;
    m_dirty = true;
}

void Actor::moveForward(float distance)
{
    glm::vec3 base(0.0f, 0.0f, 1.0f);

    base = glm::rotateX(base, m_orientation.x);
    base = glm::rotateY(base, m_orientation.y);
    base = glm::rotateZ(base, m_orientation.z);

    move(base * distance);
}

void Actor::moveRight(float distance)
{
    glm::vec3 base(-1.0f, 0.0f, 0.0f);

    base = glm::rotateX(base, m_orientation.x);
    base = glm::rotateY(base, m_orientation.y);
    base = glm::rotateZ(base, m_orientation.z);

    move(base * distance);
}

void Actor::moveLeft(float distance)
{
    moveRight(-distance);
}

void Actor::draw(const Camera* camera) const
{        
    if (m_texture) {
        m_texture->bind();
    }

    if (m_shaderProgram) {
        m_shaderProgram->use();

        GLint pMtxLoc = glGetUniformLocation(m_shaderProgram->id(), "projectionMatrix");
        GLint vMtxLoc = glGetUniformLocation(m_shaderProgram->id(), "viewMatrix");
        GLint mMtxLoc = glGetUniformLocation(m_shaderProgram->id(), "modelMatrix");

        glUniformMatrix4fv(pMtxLoc, 1, GL_FALSE, glm::value_ptr(camera->projectionMatrix()));
        glUniformMatrix4fv(vMtxLoc, 1, GL_FALSE, glm::value_ptr(camera->viewMatrix()));
        glUniformMatrix4fv(mMtxLoc, 1, GL_FALSE, glm::value_ptr(m_modelMatrix));
    } else {
        m_shaderProgram->use(false);
    }

    if (m_mesh)
        m_mesh->draw();
}

void Actor::update(float deltaTime)
{
    if (m_script)
        m_script->execute(deltaTime, this);
    
    if (m_dirty) {
        refresh();
        m_dirty = false;
    }
}

void Actor::refresh()
{
    rebuildModelMatrix();
}

void Actor::setOrientation(float x, float y, float z)
{
    m_orientation = glm::vec3(x, y, z);
    m_dirty = true;
}

void Actor::rotate(float x, float y, float z)
{
    m_orientation += glm::vec3(x, y, z);
    m_dirty = true;
}

void Actor::setScale(float s)
{
    m_scale = glm::vec3(s);
    m_dirty = true;
}
