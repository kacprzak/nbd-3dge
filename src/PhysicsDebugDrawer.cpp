#include "PhysicsDebugDrawer.h"

#include "ShaderProgram.h"
#include "Camera.h"

PhysicsDebugDrawer::PhysicsDebugDrawer()
{}

PhysicsDebugDrawer::~PhysicsDebugDrawer()
{}

//------------------------------------------------------------------------------

void PhysicsDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
    m_linesData.push_back(from.getX());
    m_linesData.push_back(from.getY());
    m_linesData.push_back(from.getZ());

    m_linesData.push_back(to.getX());
    m_linesData.push_back(to.getY());
    m_linesData.push_back(to.getZ());
}

void PhysicsDebugDrawer::drawContactPoint (const btVector3 &PointOnB, const btVector3 &normalOnB,
                                           btScalar distance, int lifeTime, const btVector3 &color)
{}

void PhysicsDebugDrawer::reportErrorWarning(const char* warningString)
{}

void PhysicsDebugDrawer::draw3dText(const btVector3& location, const char* textString)
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
    
    m_linesData.clear();
}


