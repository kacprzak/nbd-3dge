#include "PhysicsSystem.h"

#include <btBulletDynamicsCommon.h>

PhysicsSystem::PhysicsSystem()
{
    m_collisionConfiguration = new btDefaultCollisionConfiguration();
    m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
    m_overlappingPairCache = new btDbvtBroadphase();
    m_solver = new btSequentialImpulseConstraintSolver;
    m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_overlappingPairCache,
                                                  m_solver, m_collisionConfiguration);
    m_dynamicsWorld->setGravity(btVector3(0, -10, 0));
}

PhysicsSystem::~PhysicsSystem()
{
    // Remove the rigidbodies from the dynamics world and delete them
    for (int i = m_dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--) {
        btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];
        btRigidBody* body = btRigidBody::upcast(obj);
        if (body && body->getMotionState()) {
            delete body->getMotionState();
        }
        m_dynamicsWorld->removeCollisionObject(obj);
        delete obj;
    }

    // Delete collision shapes
    for (int j = 0; j < m_collisionShapes.size(); j++) {
        btCollisionShape* shape = m_collisionShapes[j];
        m_collisionShapes[j] = 0;
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

    for (int j = m_dynamicsWorld->getNumCollisionObjects() - 1; j >= 0; j--) {
        btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[j];
        btRigidBody* body = btRigidBody::upcast(obj);
        btTransform trans;
        if (body && body->getMotionState()) {
            body->getMotionState()->getWorldTransform(trans);
        }
        else {
            trans = obj->getWorldTransform();
        }
        TransformationComponent* tr = (TransformationComponent*)body->getUserPointer();
        tr->position.x = trans.getOrigin().getX();
        tr->position.y = trans.getOrigin().getY();
        tr->position.z = trans.getOrigin().getZ();
    }
}

void PhysicsSystem::addActor(int id, TransformationComponent *tr, RenderComponent *rd)
{
    //btCollisionShape* colShape = new btBoxShape(btVector3(1,1,1));
    btCollisionShape* colShape = new btSphereShape(btScalar(1.));
    m_collisionShapes.push_back(colShape);

    // Create Dynamic Objects
    btTransform startTransform;
    startTransform.setIdentity();

    btScalar mass(1.f);
    if (rd->role == Role::Terrain)
        mass = 0.f;

    // Rigidbody is dynamic if and only if mass is non zero, otherwise static
    bool isDynamic = (mass != 0.f);

    btVector3 localInertia(0, 0, 0);
    if (isDynamic)
        colShape->calculateLocalInertia(mass, localInertia);

    const auto& p = tr->position;
    startTransform.setOrigin(btVector3(p.x, p.y, p.z));

    // Using motionstate is recommended, it provides interpolation capabilities, and only
    // synchronizes 'active' objects
    btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
    btRigidBody* body = new btRigidBody(rbInfo);

    body->setUserPointer((void*)tr);

    m_dynamicsWorld->addRigidBody(body);
}
