#include "PhysicsSystem.h"

#include <BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>
#include <btBulletDynamicsCommon.h>
// Fixme: remove this dependency
#include "ObjLoader.h"
#include "Terrain.h"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>

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

    // Delete collision shapes
    for (int j = 0; j < m_collisionShapes.size(); j++) {
        btCollisionShape* shape = m_collisionShapes[j];
        m_collisionShapes[j]    = 0;
        delete shape;
    }

    delete m_dynamicsWorld;
    delete m_solver;
    delete m_overlappingPairCache;
    delete m_dispatcher;
    delete m_collisionConfiguration;
}

void PhysicsSystem::update(float elapsedTime)
{
    m_dynamicsWorld->stepSimulation(elapsedTime);

    for (int i = m_dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; --i) {
        btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];
        btRigidBody* body      = btRigidBody::upcast(obj);
        btTransform trans;
        if (body && body->getMotionState()) {
            body->getMotionState()->getWorldTransform(trans);
        } else {
            trans = obj->getWorldTransform();
        }
        // Sync component
        TransformationComponent* tr = (TransformationComponent*)body->getUserPointer();
        tr->position.x              = trans.getOrigin().getX();
        tr->position.y              = trans.getOrigin().getY();
        tr->position.z              = trans.getOrigin().getZ();

        tr->orientation.x = trans.getRotation().getX();
        tr->orientation.y = trans.getRotation().getY();
        tr->orientation.z = trans.getRotation().getZ();
        tr->orientation.w = trans.getRotation().getW();
    }
}

void PhysicsSystem::addActor(int id, TransformationComponent* tr, PhysicsComponent* ph,
                             const std::string& dataFolder)
{
    btCollisionShape* colShape = nullptr;
    const std::string shape    = ph->shape;

    if (!shape.empty()) {
        std::vector<std::string> splitted;
        boost::split(splitted, shape, boost::is_any_of(":"));
        if (splitted.size() != 2) throw std::runtime_error{"Invalid shape format: " + shape};

        if (splitted[0] == "heightfield") {
            int w, h;
            float amp       = 40.f;
            m_heightmapData = Terrain::getHeightData(dataFolder + splitted[1], &w, &h, amp);
            colShape        = new btHeightfieldTerrainShape{
                w, h, m_heightmapData.data(), 1.0f, -amp, amp, 1, PHY_FLOAT, false};

        } else if (splitted[0] == "mesh") {
            auto convexHullShape = new btConvexHullShape;

            ObjLoader loader;
            loader.load(dataFolder + splitted[1]);
            const auto& v = loader.vertices();
            for (size_t i = 0; i < v.size(); i = i + 3) {
                convexHullShape->addPoint(btVector3{v[i], v[i + 1], v[i + 2]}, false);
            }
            convexHullShape->recalcLocalAabb();
            colShape = convexHullShape;

        } else {
            throw std::runtime_error{"Unknown shape type: " + splitted[0]};
        }
    } else {
        colShape = new btBoxShape(btVector3(1, 1, 1));
        // colShape = new btSphereShape(btScalar(10.));
    }
    colShape->setLocalScaling({tr->scale, tr->scale, tr->scale});
    m_collisionShapes.push_back(colShape);

    // Create Dynamic Objects
    btTransform startTransform;
    startTransform.setIdentity();

    btScalar mass(ph->mass);

    // Rigidbody is dynamic if and only if mass is non zero, otherwise static
    bool isDynamic = (mass != 0.f);

    btVector3 localInertia(0, 0, 0);
    if (isDynamic) colShape->calculateLocalInertia(mass, localInertia);

    const auto& p = tr->position;
    startTransform.setOrigin(btVector3(p.x, p.y, p.z));

    const auto& orient = tr->orientation;
    startTransform.setRotation(btQuaternion(orient.x, orient.y, orient.z, orient.w));

    // Using motionstate is recommended, it provides interpolation capabilities, and only
    // synchronizes 'active' objects
    btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
    btRigidBody* body = new btRigidBody(rbInfo);

    body->setUserPointer((void*)tr);

    m_dynamicsWorld->addRigidBody(body);
}

void PhysicsSystem::setDebugDrawer(btIDebugDraw* debugDrawer)
{
    debugDrawer->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
    m_dynamicsWorld->setDebugDrawer(debugDrawer);
}

void PhysicsSystem::drawDebugData() { m_dynamicsWorld->debugDrawWorld(); }
