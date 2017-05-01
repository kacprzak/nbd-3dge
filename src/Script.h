#ifndef SCRIPT_H
#define SCRIPT_H

#include "Actor.h"

class Script
{
 public:
    virtual void execute(float deltaTime, Actor* actor) = 0;
};

#endif // SCRIPT_H
