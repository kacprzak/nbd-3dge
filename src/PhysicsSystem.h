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

class PhysicsSystem final : private boost::noncopyable
{
  public:
    PhysicsSystem();
    ~PhysicsSystem();

    void update(float elapsedTime);

    void addActor(int id, TransformationComponent* tr, RenderComponent* rd);
    void removeActor(int id);

    //virtual void toggleDrawDebug() = 0;
    //virtual void drawDebugData()   = 0;

 private:
    btCollisionConfiguration* m_collisionConfiguration;
    btDispatcher* m_dispatcher;
    btBroadphaseInterface* m_overlappingPairCache;
    btSequentialImpulseConstraintSolver* m_solver;
    btDynamicsWorld* m_dynamicsWorld;

    btAlignedObjectArray<btCollisionShape*> m_collisionShapes;
};

#endif // PHYSICSSYSTEM_H
