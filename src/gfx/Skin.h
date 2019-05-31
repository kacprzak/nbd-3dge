#ifndef GFX_SKIN_H
#define GFX_SKIN_H

#include "Buffer.h"

#include <string>

namespace gfx {

class Skin
{
  public:
    Accessor m_inverseBindMatrices;
    std::vector<int> m_joints; //< Nodes representing joints transformation
    int m_skeleton; //< Root node of joints hierarchy

    std::string name;
};

} // namespace gfx

#endif // GFX_SKIN_H
