#ifndef PHYSICSDEBUGDRAWER_H
#define PHYSICSDEBUGDRAWER_H

#include <LinearMath/btIDebugDraw.h>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

class ShaderProgram;
class Camera;

class PhysicsDebugDrawer final : public btIDebugDraw
{
  public:
    PhysicsDebugDrawer();
    ~PhysicsDebugDrawer();

    void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;
    void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance,
                          int lifeTime, const btVector3& color) override;
    void reportErrorWarning(const char* warningString) override;
    void draw3dText(const btVector3& location, const char* textString) override;
    void setDebugMode(int debugMode) override;
    int getDebugMode() const override;

    void draw(Camera* camera);

  private:
    void updateBuffer();

    int m_debugMode;
    std::vector<float> m_linesData;

    GLuint m_vao                     = 0;
    GLuint m_buffer                  = 0;
    std::size_t m_bufferReservedSize = 0;

    std::unique_ptr<ShaderProgram> m_shaderProgram;
    glm::mat4 m_lastMVP{1.f}; //< ModelViewProjection matrix from previous frame
};

#endif // PHYSICSDEBUGDRAWER_H
