#ifndef PHYSICSDEBUGDRAWER_H
#define PHYSICSDEBUGDRAWER_H

#include <LinearMath/btIDebugDraw.h>

#include <vector>

class ShaderProgram;
class Camera;

class PhysicsDebugDrawer : public btIDebugDraw
{
 public:
    PhysicsDebugDrawer();
    ~PhysicsDebugDrawer();

    void drawLine(const btVector3& from,const btVector3& to,const btVector3& color) override;
    void drawContactPoint (const btVector3 &PointOnB, const btVector3 &normalOnB,
                           btScalar distance, int lifeTime, const btVector3 &color) override;
    void reportErrorWarning(const char* warningString) override;
    void draw3dText(const btVector3& location,const char* textString) override;
    void setDebugMode(int debugMode) override;
    int getDebugMode() const override;

    void draw(ShaderProgram* shaderProgram, Camera* camera);

 private:
    int m_debugMode;
    std::vector<float> m_linesData;
};

#endif // PHYSICSDEBUGDRAWER_H
