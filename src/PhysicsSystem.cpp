#include "PhysicsSystem.h"

#include <BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>
#include <btBulletDynamicsCommon.h>
// Fixme: remove this dependency
#include "ObjLoader.h"
#include "Terrain.h"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/lexical_cast.hpp>

class MotionState : public btMotionState
{
  public:
    MotionState(TransformationComponent* tr)
        : m_tr{tr}
    {
    }

    void getWorldTransform(btTransform& worldTrans) const override
    {
        const auto& p = m_tr->position;
        worldTrans.setOrigin({p.x, p.y, p.z});
        const auto& o = m_tr->orientation;
        worldTrans.setRotation({o.x, o.y, o.z, o.w});
    }

    void setWorldTransform(const btTransform& worldTrans) override
    {
        m_tr->position.x = worldTrans.getOrigin().x();
        m_tr->position.y = worldTrans.getOrigin().y();
        m_tr->position.z = worldTrans.getOrigin().z();

        m_tr->orientation.x = worldTrans.getRotation().x();
        m_tr->orientation.y = worldTrans.getRotation().y();
        m_tr->orientation.z = worldTrans.getRotation().z();
        m_tr->orientation.w = worldTrans.getRotation().w();
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
    m_dynamicsWorld->setGravity(btVector3(0, -10, 0));
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

void PhysicsSystem::update(float elapsedTime) { m_dynamicsWorld->stepSimulation(elapsedTime); }

//------------------------------------------------------------------------------

void PhysicsSystem::addActor(int id, TransformationComponent* tr, PhysicsComponent* ph,
                             const std::string& dataFolder)
{
    auto colShape = createCollisionShape(*ph, dataFolder);
    colShape->setLocalScaling({tr->scale, tr->scale, tr->scale});

    // Create Dynamic Objects
    btScalar mass(ph->mass);
    // Rigidbody is dynamic if and only if mass is non zero, otherwise static
    bool isDynamic = (mass != 0.f);

    btVector3 localInertia(0, 0, 0);
    if (isDynamic) colShape->calculateLocalInertia(mass, localInertia);

    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, new MotionState{tr}, colShape.get(),
                                                    localInertia);

    m_collisionShapes.push_back(std::move(colShape));
    btRigidBody* body = new btRigidBody(rbInfo);
    m_dynamicsWorld->addRigidBody(body);
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

std::unique_ptr<btCollisionShape> PhysicsSystem::createCollisionShape(const PhysicsComponent& ph,
                                                                      const std::string& dataFolder)
{
    const std::string shape = ph.shape;

    if (!shape.empty()) {
        std::vector<std::string> splitted;
        boost::split(splitted, shape, boost::is_any_of(":"));
        if (splitted.size() < 2) throw std::runtime_error{"Invalid shape format: " + shape};

        if (splitted[0] == "heightfield") {
            int w, h;
            float amp       = 40.f;
            m_heightmapData = Terrain::getHeightData(dataFolder + splitted[1], &w, &h, amp);
            auto colShape   = std::make_unique<btHeightfieldTerrainShape>(
                w, h, m_heightmapData.data(), 1.0f, -amp, amp, 1, PHY_FLOAT, false);
            return colShape;

        } else if (splitted[0] == "mesh") {
            auto colShape = std::make_unique<btConvexHullShape>();

            ObjLoader loader;
            loader.load(dataFolder + splitted[1]);
            const auto& v = loader.vertices();
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
                boost::lexical_cast<float>(splitted[1]), boost::lexical_cast<float>(splitted[2]),
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
