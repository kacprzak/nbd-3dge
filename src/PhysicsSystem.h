#ifndef PHYSICSSYSTEM_H
#define PHYSICSSYSTEM_H

#include "Components.h"
#include "ResourcesMgr.h"

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
class btRigidBody;

class PhysicsSystem final : private boost::noncopyable
{
    struct PhysicsNode
    {
        int actorId;
        TransformationComponent* tr;
        PhysicsComponent* ph;
        btRigidBody* body;
    };

  public:
    PhysicsSystem();
    ~PhysicsSystem();

    void update(float elapsedTime);

    void addActor(int id, TransformationComponent* tr, PhysicsComponent* ph,
                  const ResourcesMgr& resourcesMgr);
    void removeActor(int id);

    void setDebugDrawer(btIDebugDraw* debugDrawer);
    void drawDebugData();

  private:
    std::unique_ptr<btCollisionShape> createCollisionShape(const PhysicsComponent& ph,
                                                           const ResourcesMgr& resourcesMgr);

    btCollisionConfiguration* m_collisionConfiguration;
    btDispatcher* m_dispatcher;
    btBroadphaseInterface* m_overlappingPairCache;
    btSequentialImpulseConstraintSolver* m_solver;
    btDynamicsWorld* m_dynamicsWorld;

    using ShapesKey = std::pair<std::string, float>;
    std::map<ShapesKey, std::unique_ptr<btCollisionShape>> m_collisionShapes;

    std::vector<PhysicsNode> m_nodes;
};

#endif // PHYSICSSYSTEM_H
