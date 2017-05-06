#include "Actor.h"

#include "Logger.h"

Actor::Actor(ActorId id)
    : m_id(id)
    , m_dead(false)
{
    LOG_TRACE << "new Actor: id = " << m_id;
}

//------------------------------------------------------------------------------

Actor::~Actor() { LOG_TRACE << "delete Actor: id = " << m_id; }

//------------------------------------------------------------------------------

void Actor::die() { m_dead = true; }
