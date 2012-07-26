#include "actor.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

Actor::Actor(const std::string& name, MeshPtr mesh)
    : m_name(name)
    , m_mesh(mesh)
    , m_state(Idle)
    , m_hasTexture(false)
    , m_position(glm::vec3(0.0f))
    , m_orientation(glm::vec3(0.0f))
    , m_scale(glm::vec3(1.0f))
    , m_modelMatrix(glm::mat4(1.0f))
{
}

Actor::Actor(const std::string& name)
    : m_name(name)
    , m_mesh()
    , m_state(Idle)
    , m_hasTexture(false)
    , m_position(glm::vec3(0.0f))
    , m_orientation(glm::vec3(0.0f))
    , m_scale(glm::vec3(1.0f))
    , m_modelMatrix(glm::mat4(1.0f))
{
}

void Actor::setTexture(TexturePtr tex)
{
    m_texture = tex;
    m_hasTexture = true;
}

void Actor::setMesh(MeshPtr mesh)
{
    m_mesh = mesh;
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
    rebuildModelMatrix();
}

void Actor::move(const glm::vec3& pos)
{
    m_position += pos;
    rebuildModelMatrix();
}

void Actor::moveTo(float x, float y, float z)
{
    m_position = glm::vec3(x, y, z);
    rebuildModelMatrix();
}

void Actor::moveTo(const glm::vec3& pos)
{
    m_position = pos;
    rebuildModelMatrix();
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

void Actor::update(float /* delta */)
{
    // do nothing
}

void Actor::setOrientation(float x, float y, float z)
{
    m_orientation = glm::vec3(x, y, z);
    rebuildModelMatrix();
}

void Actor::rotate(float x, float y, float z)
{
    m_orientation += glm::vec3(x, y, z);
    rebuildModelMatrix();
}

void Actor::setScale(float s)
{
    m_scale = glm::vec3(s);
    rebuildModelMatrix();
}
