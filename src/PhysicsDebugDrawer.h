#ifndef PHYSICSDEBUGDRAWER_H
#define PHYSICSDEBUGDRAWER_H

#include <LinearMath/btIDebugDraw.h>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace gfx {
class ShaderProgram;
class Camera;
} // namespace gfx

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

    void draw(gfx::Camera* camera);

  private:
    void updateBuffer();

    struct Line
    {
        glm::vec3 pos1;
        glm::vec3 col1;
        glm::vec3 pos2;
        glm::vec3 col2;
    };

    int m_debugMode{};
    std::vector<Line> m_linesData;
    size_t m_currLinesDataIdx       = 0;
    size_t m_requestedLinesDataSize = 0;

    GLuint m_vao                     = 0;
    GLuint m_buffer                  = 0;
    std::size_t m_bufferReservedSize = 0;

    std::unique_ptr<gfx::ShaderProgram> m_shaderProgram;
};

#endif // PHYSICSDEBUGDRAWER_H
