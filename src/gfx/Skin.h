#ifndef GFX_SKIN_H
#define GFX_SKIN_H

#include "../Logger.h"
#include "Buffer.h"

#include <string>

namespace gfx {

class Node;

class Skin
{
  public:
    void setIBMatrix(const Accessor& ibm) { m_inverseBindMatrices = ibm; }
    void setJoints(const std::vector<unsigned> joints, int root)
    {
        std::copy(std::cbegin(joints), std::cend(joints), std::back_inserter(m_joints));
        m_skeleton = root;

        if (joints.size() > 20)
            LOG_WARNING("Skeleotn with {} joints. Only {} supprted.", joints.size(), 20);
    }

    void calculateJointMatrices(std::array<glm::mat4, 20>& jointMatrices, const Node* node) const;

    std::string name;

  private:
    Accessor m_inverseBindMatrices;
    std::vector<int> m_joints; //< Nodes representing joints transformation
    int m_skeleton;            //< Root node of joints hierarchy
};

} // namespace gfx

#endif // GFX_SKIN_H
