#include "actor.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Actor::Actor(boost::shared_ptr<Mesh> mesh)
    : m_mesh(mesh)
    , m_state(Idle)
    , m_hasTexture(false)
    , m_modelMatrix(glm::mat4(1.0f))
{
    m_color[0] = 1.0f;
    m_color[1] = 1.0f;
    m_color[2] = 1.0f;
}

Actor::Actor()
  : m_mesh()
  , m_state(Idle)
  , m_hasTexture(false)
{
    m_color[0] = 1.0f;
    m_color[1] = 1.0f;
    m_color[2] = 1.0f;
}

void Actor::setTexture(boost::shared_ptr<Texture> tex)
{
    m_texture = tex;
    m_hasTexture = true;
}

void Actor::setMesh(boost::shared_ptr<Mesh> mesh)
{
    m_mesh = mesh;
}

void Actor::move(const glm::vec3& posDelta)
{
    m_modelMatrix = glm::translate(m_modelMatrix, posDelta);
}

void Actor::moveTo(const glm::vec3& pos)
{
    m_modelMatrix[3][0] = 0.0f;
    m_modelMatrix[3][1] = 0.0f;
    m_modelMatrix[3][2] = 0.0f;
    m_modelMatrix = glm::translate(m_modelMatrix, pos);
}

void Actor::draw() const
{
    if (m_hasTexture) {
        m_texture->bind();
    }

    m_mesh->draw();
}

void Actor::draw(ShaderProgram *program) const
{
    if (program) {
        GLint modelMatrixUnif = glGetUniformLocation(program->id(), "modelMatrix");
        glUniformMatrix4fv(modelMatrixUnif, 1, GL_FALSE, glm::value_ptr(m_modelMatrix));
    }

    draw();
}

void Actor::update(float delta)
{
    rotate(0.0f, delta * 100.0f, 0.0f);
}

void Actor::setOrientation(float x, float y, float z)
{
//    m_orientation[0] = x;
//    m_orientation[1] = y;
//    m_orientation[2] = z;
}

void Actor::rotate(float x, float y, float z)
{
//    m_orientation[0] += x;
//    m_orientation[1] += y;
//    m_orientation[2] += z;
}

void Actor::setScale(float s)
{
    m_modelMatrix = glm::scale(m_modelMatrix, glm::vec3(s));
}

void Actor::setScale(float x, float y, float z)
{
    m_modelMatrix = glm::scale(m_modelMatrix, glm::vec3(x, y, z));
}
