#ifndef SCRIPT_H
#define SCRIPT_H

class GfxNode;

class Script
{
 public:
    virtual void execute(float deltaTime, GfxNode* actor) = 0;
};

#endif // SCRIPT_H
