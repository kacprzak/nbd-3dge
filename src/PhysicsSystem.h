#ifndef PHYSICSSYSTEM_H
#define PHYSICSSYSTEM_H

#include "Components.h"

#include <LinearMath/btAlignedObjectArray.h>
#include <boost/noncopyable.hpp>

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

    void addActor(int id, TransformationComponent* tr, RenderComponent* rd);
    void removeActor(int id);

    void setDebugDrawer(btIDebugDraw* debugDrawer);
    void drawDebugData();

 private:
    btCollisionConfiguration* m_collisionConfiguration;
    btDispatcher* m_dispatcher;
    btBroadphaseInterface* m_overlappingPairCache;
    btSequentialImpulseConstraintSolver* m_solver;
    btDynamicsWorld* m_dynamicsWorld;

    btAlignedObjectArray<btCollisionShape*> m_collisionShapes;
};

#endif // PHYSICSSYSTEM_H
