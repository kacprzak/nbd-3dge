#ifndef PHYSICSSYSTEM_H
#define PHYSICSSYSTEM_H

#include <boost/noncopyable.hpp>

class PhysicsSystem : private boost::noncopyable
{
  public:
    virtual ~PhysicsSystem() {}

    virtual void update(float elapsedTime);

    //virtual void toggleDrawDebug() = 0;
    //virtual void drawDebugData()   = 0;

    // virtual void applyForce(const Vec2& dir, float newtons, ActorId actorId)
    // = 0;
};

#endif // PHYSICSSYSTEM_H
