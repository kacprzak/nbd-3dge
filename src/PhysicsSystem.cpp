#include "PhysicsSystem.h"

#include "Logger.h"

#include <BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>
#include <btBulletDynamicsCommon.h>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/lexical_cast.hpp>

class MotionState : public btMotionState
{
  public:
    explicit MotionState(TransformationComponent* tr)
        : m_tr{tr}
    {
    }

    void getWorldTransform(btTransform& worldTrans) const override
    {
        const auto& p = m_tr->translation;
        worldTrans.setOrigin({p.x, p.y, p.z});
        const auto& o = m_tr->rotation;
        worldTrans.setRotation({o.x, o.y, o.z, o.w});
    }

    void setWorldTransform(const btTransform& worldTrans) override
    {
        m_tr->translation.x = worldTrans.getOrigin().x();
        m_tr->translation.y = worldTrans.getOrigin().y();
        m_tr->translation.z = worldTrans.getOrigin().z();

        m_tr->rotation.x = worldTrans.getRotation().x();
        m_tr->rotation.y = worldTrans.getRotation().y();
        m_tr->rotation.z = worldTrans.getRotation().z();
        m_tr->rotation.w = worldTrans.getRotation().w();
    }

  private:
    TransformationComponent* m_tr;
};

//==============================================================================

PhysicsSystem::PhysicsSystem()
{
    m_collisionConfiguration = new btDefaultCollisionConfiguration();
    m_dispatcher             = new btCollisionDispatcher(m_collisionConfiguration);
    m_overlappingPairCache   = new btDbvtBroadphase();
    m_solver                 = new btSequentialImpulseConstraintSolver;
    m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_overlappingPairCache, m_solver,
                                                  m_collisionConfiguration);
    m_dynamicsWorld->setGravity(btVector3(0, -9.81, 0));
}

//------------------------------------------------------------------------------

PhysicsSystem::~PhysicsSystem()
{
    // Remove the rigidbodies from the dynamics world and delete them
    for (int i = m_dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; --i) {
        btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];
        btRigidBody* body      = btRigidBody::upcast(obj);
        if (body && body->getMotionState()) {
            delete body->getMotionState();
        }
        m_dynamicsWorld->removeCollisionObject(obj);
        delete obj;
    }

    delete m_dynamicsWorld;
    delete m_solver;
    delete m_overlappingPairCache;
    delete m_dispatcher;
    delete m_collisionConfiguration;
}

//------------------------------------------------------------------------------

void PhysicsSystem::update(float elapsedTime)
{
    for (auto& n : m_nodes) {
        if (glm::length(n.ph->force) > 0.f) {
            n.body->activate();
            auto force = glm::rotate(n.tr->rotation, n.ph->force);
            n.body->applyCentralForce(btVector3{force.x, force.y, force.z});
        }
        if (glm::length(n.ph->torque) > 0.f) {
            n.body->activate();
            auto torque = glm::rotate(n.tr->rotation, n.ph->torque);
            n.body->applyTorque(btVector3{torque.x, torque.y, torque.z});
        }
    }
    m_dynamicsWorld->stepSimulation(elapsedTime);
}

//------------------------------------------------------------------------------

void PhysicsSystem::addActor(int id, TransformationComponent* tr, PhysicsComponent* ph,
                             const ResourcesMgr& resourcesMgr)
{
    btCollisionShape* colShape = nullptr;

    auto key        = std::make_pair(ph->shape, tr->scale[0]);
    auto colShapeIt = m_collisionShapes.find(key);
    if (colShapeIt == std::end(m_collisionShapes)) {
        auto colShapeUPtr = createCollisionShape(*ph, resourcesMgr);
        colShapeUPtr->setLocalScaling({tr->scale[0], tr->scale[1], tr->scale[2]});
        colShape               = colShapeUPtr.get();
        m_collisionShapes[key] = std::move(colShapeUPtr);
        LOG_TRACE("Created CollisionShape: {} {}", key.first, key.second);
    } else {
        colShape = colShapeIt->second.get();
    }

    // Create Dynamic Objects
    btScalar mass(ph->mass);
    // Rigidbody is dynamic if and only if mass is non zero, otherwise static
    bool isDynamic = (mass != 0.f);

    btVector3 localInertia(0, 0, 0);
    if (isDynamic) colShape->calculateLocalInertia(mass, localInertia);

    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, new MotionState{tr}, colShape,
                                                    localInertia);

    PhysicsNode node;
    node.actorId = id;
    node.tr      = tr;
    node.ph      = ph;
    node.body    = new btRigidBody(rbInfo);

    m_dynamicsWorld->addRigidBody(node.body);
    m_nodes.push_back(node);
}

//------------------------------------------------------------------------------

void PhysicsSystem::removeActor(int id)
{
    auto nodeIt = std::find_if(m_nodes.begin(), m_nodes.end(),
                               [id](const PhysicsNode& n) { return n.actorId == id; });
    if (nodeIt != std::end(m_nodes)) {
        auto& node = *nodeIt;
        delete node.body->getMotionState();
        m_dynamicsWorld->removeRigidBody(node.body);
        delete node.body;
        m_nodes.erase(nodeIt);
    }
}

//------------------------------------------------------------------------------

void PhysicsSystem::setDebugDrawer(btIDebugDraw* debugDrawer)
{
    debugDrawer->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
    m_dynamicsWorld->setDebugDrawer(debugDrawer);
}

//------------------------------------------------------------------------------

void PhysicsSystem::drawDebugData() { m_dynamicsWorld->debugDrawWorld(); }

//------------------------------------------------------------------------------

std::unique_ptr<btCollisionShape>
PhysicsSystem::createCollisionShape(const PhysicsComponent& ph, const ResourcesMgr& resourcesMgr)
{
    const std::string shape = ph.shape;

    if (!shape.empty()) {
        std::vector<std::string> splitted;
        boost::split(splitted, shape, boost::is_any_of(":"));
        if (splitted.size() < 2) throw std::runtime_error{"Invalid shape format: " + shape};

        if (splitted[0] == "heightfield") {
            const auto& heightfield = resourcesMgr.getHeightfield(splitted[1]);
            auto colShape           = std::make_unique<btHeightfieldTerrainShape>(
                heightfield->w, heightfield->h, heightfield->heights.data(), 1.0f,
                -heightfield->amplitude, heightfield->amplitude, 1, PHY_FLOAT, false);
            return colShape;

        } else if (splitted[0] == "mesh") {
            auto colShape = std::make_unique<btConvexHullShape>();

            const auto& v = resourcesMgr.getMesh(splitted[1])->positions();
            for (size_t i = 0; i < v.size(); i = i + 3) {
                colShape->addPoint(btVector3{v[i], v[i + 1], v[i + 2]}, false);
            }
            colShape->recalcLocalAabb();
            return colShape;

        } else if (splitted[0] == "capsule") {
            auto colShape = std::make_unique<btCapsuleShape>(
                boost::lexical_cast<float>(splitted[1]), boost::lexical_cast<float>(splitted[2]));
            return colShape;

        } else if (splitted[0] == "box") {
            auto colShape = std::make_unique<btBoxShape>(btVector3{
                boost::lexical_cast<float>(splitted[1]),
                boost::lexical_cast<float>(splitted[2]),
                boost::lexical_cast<float>(splitted[3]),
            });
            return colShape;

        } else {
            throw std::runtime_error{"Unknown shape type: " + splitted[0]};
        }
    } else {
        // colShape = new btSphereShape(btScalar(10.));
        throw std::runtime_error{"Shape not defined"};
    }

    return {};
}
