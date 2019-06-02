#include "Skin.h"

#include "Model.h"

namespace gfx {

void Skin::calculateJointMatrices(std::array<glm::mat4, 20>& jointMatrices, const Node* node) const
{
    const Model* model = node->getModel();

    for (std::ptrdiff_t i = 0; i < jointMatrices.size() && i < m_joints.size(); ++i) {
        const Node* n       = model->getNode(m_joints[i]);
        const glm::mat4 ibm = m_inverseBindMatrices.getElements<glm::mat4>(i, i + 1)[0];
        jointMatrices[i]    = glm::inverse(node->getWorldMatrix()) * n->getWorldMatrix() * ibm;
    }
}

} // namespace gfx
