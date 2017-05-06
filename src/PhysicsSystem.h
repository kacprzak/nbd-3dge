#ifndef PHYSICSSYSTEM_H
#define PHYSICSSYSTEM_H

#include "Components.h"

#include <LinearMath/btAlignedObjectArray.h>
#include <boost/noncopyable.hpp>
#include <memory>

class btCollisionConfiguration;
class btDispatcher;
class btBroadphaseInterface;
class btSequentialImpulseConstraintSolver;
class btDynamicsWorld;
class btCollisionShape;
class btIDebugDraw;

class PhysicsSystem final : private boost::noncopyable
{
  public:
    PhysicsSystem();
    ~PhysicsSystem();

    void update(float elapsedTime);

    void addActor(int id, TransformationComponent* tr, PhysicsComponent* ph,
                  const std::string& dataFolder);
    void removeActor(int id);

    void setDebugDrawer(btIDebugDraw* debugDrawer);
    void drawDebugData();

  private:
    std::unique_ptr<btCollisionShape> createCollisionShape(const PhysicsComponent& ph,
                                                           const std::string& dataFolder);

    btCollisionConfiguration* m_collisionConfiguration;
    btDispatcher* m_dispatcher;
    btBroadphaseInterface* m_overlappingPairCache;
    btSequentialImpulseConstraintSolver* m_solver;
    btDynamicsWorld* m_dynamicsWorld;

    std::vector<std::unique_ptr<btCollisionShape>> m_collisionShapes;
    std::vector<float> m_heightmapData;
};

#endif // PHYSICSSYSTEM_H
