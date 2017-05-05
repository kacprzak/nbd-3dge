#include "PhysicsDebugDrawer.h"

#include "ShaderProgram.h"
#include "Camera.h"
#include "Logger.h"

static std::string vertexSource = R"==(
#version 430
uniform mat4 MVP;

out vec4 color;

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_color;

void main()
{
    gl_Position = MVP * vec4(in_position, 1.0);
    color = vec4(in_color, 1.0);
}
)==";

static std::string fragmentSource = R"==(
#version 430
in vec4 color;
out vec4 fragColor;

void main()
{
    fragColor = color;
}
)==";

//==============================================================================

PhysicsDebugDrawer::PhysicsDebugDrawer()
{
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_buffer);

    m_shaderProgram = std::make_unique<ShaderProgram>();
    Shader vertex{GL_VERTEX_SHADER, vertexSource.c_str()};
    m_shaderProgram->addShared(&vertex);
    Shader fragment{GL_FRAGMENT_SHADER, fragmentSource.c_str()};
    m_shaderProgram->addShared(&fragment);

    m_shaderProgram->link();
}

PhysicsDebugDrawer::~PhysicsDebugDrawer()
{
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_buffer);
}

//------------------------------------------------------------------------------

// Speeds up debug rendering in release. Slows down in debug.
#ifdef NDEBUG
#define EARLY_CLIPPING
#endif

void PhysicsDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
#ifdef EARLY_CLIPPING
    auto from_homo = m_lastMVP * glm::vec4{from.x(), from.y(), from.z(), 1.f};
    from_homo = from_homo / from_homo.w;
    if (from_homo.x > 1.f || from_homo.x < -1.f
        || from_homo.y > 1.f || from_homo.y < -1.f
        || from_homo.z > 1.f || from_homo.z < -1.f) {
        return;
    }

    auto to_homo = m_lastMVP * glm::vec4{to.x(), to.y(), to.z(), 1.f};
    to_homo = to_homo / to_homo.w;
    if (to_homo.x > 1.f || to_homo.x < -1.f
        || to_homo.y > 1.f || to_homo.y < -1.f
        || to_homo.z > 1.f || to_homo.z < -1.f) {
        return;
    }
#endif
    
    m_linesData.push_back(from.x());
    m_linesData.push_back(from.y());
    m_linesData.push_back(from.z());

    m_linesData.push_back(color.x());
    m_linesData.push_back(color.y());
    m_linesData.push_back(color.z());
    
    m_linesData.push_back(to.x());
    m_linesData.push_back(to.y());
    m_linesData.push_back(to.z());

    m_linesData.push_back(color.x());
    m_linesData.push_back(color.y());
    m_linesData.push_back(color.z());
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

void PhysicsDebugDrawer::draw(Camera* camera)
{
    m_lastMVP = camera->projectionMatrix() * camera->viewMatrix();

    if (m_linesData.empty())
        return;
    updateBuffer();

    m_shaderProgram->use();
    m_shaderProgram->setUniform("MVP", m_lastMVP);

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
        LOG_TRACE << "PhysicsDebugDrawer::m_bufferReservedSize = " << m_bufferReservedSize;
    }

    glBufferSubData(GL_ARRAY_BUFFER, 0, bufferSize, &m_linesData[0]);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
}


