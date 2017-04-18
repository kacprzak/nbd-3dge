#ifndef TERRAIN_H
#define TERRAIN_H

#include "Actor.h"

class Terrain : public Actor
{
 public:
    Terrain(const std::string& name, const std::string& heightMapPath);
};

#endif // TERRAIN_H
