#include "PhysicsDebugDrawer.h"

#include "ShaderProgram.h"
#include "Camera.h"

PhysicsDebugDrawer::PhysicsDebugDrawer()
{
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_buffer);
}

PhysicsDebugDrawer::~PhysicsDebugDrawer()
{
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_buffer);
}

//------------------------------------------------------------------------------

void PhysicsDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
    m_linesData.push_back(from.getX());
    m_linesData.push_back(from.getY());
    m_linesData.push_back(from.getZ());

    m_linesData.push_back(color.getX());
    m_linesData.push_back(color.getY());
    m_linesData.push_back(color.getZ());
    
    m_linesData.push_back(to.getX());
    m_linesData.push_back(to.getY());
    m_linesData.push_back(to.getZ());

    m_linesData.push_back(color.getX());
    m_linesData.push_back(color.getY());
    m_linesData.push_back(color.getZ());
}

void PhysicsDebugDrawer::drawContactPoint (const btVector3& /*PointOnB*/, const btVector3& /*normalOnB*/,
                                           btScalar /*distance*/, int /*lifeTime*/,
                                           const btVector3& /*color*/)
{}

void PhysicsDebugDrawer::reportErrorWarning(const char* /*warningString*/)
{}

void PhysicsDebugDrawer::draw3dText(const btVector3& /*location*/, const char* /*textString*/)
{}

void PhysicsDebugDrawer::setDebugMode(int debugMode)
{
    m_debugMode = debugMode;
}

int PhysicsDebugDrawer::getDebugMode() const
{
    return m_debugMode;
}

//------------------------------------------------------------------------------

void PhysicsDebugDrawer::draw(ShaderProgram* shaderProgram, Camera* camera)
{
    if (m_linesData.empty())
        return;
    updateBuffer();

    if (shaderProgram) {
        shaderProgram->use();
        shaderProgram->setUniform("projectionMatrix", camera->projectionMatrix());
        shaderProgram->setUniform("viewMatrix", camera->viewMatrix());
        shaderProgram->setUniform("modelMatrix", glm::mat4(1.f));
    }

    glBindVertexArray(m_vao);
    glDrawArrays(GL_LINES, 0, m_linesData.size() / 6);
    
    m_linesData.clear();
}

void PhysicsDebugDrawer::updateBuffer()
{
    size_t bufferSize = m_linesData.size() * sizeof(float);
    
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_buffer);

    if (bufferSize > m_bufferReservedSize) {
        glBufferData(GL_ARRAY_BUFFER, bufferSize, 0, GL_DYNAMIC_DRAW);
        m_bufferReservedSize = bufferSize;
    }

    glBufferSubData(GL_ARRAY_BUFFER, 0, bufferSize, &m_linesData[0]);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
}


