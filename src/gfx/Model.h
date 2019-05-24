#ifndef GFX_MODEL_H
#define GFX_MODEL_H

#include "Buffer.h"
#include "Camera.h"
#include "Mesh.h"
#include "Animation.h"
#include "Node.h"
#include "Texture.h"

namespace loaders {
class GltfLoader;
} // namespace loaders

namespace gfx {

class Model
{
    friend class loaders::GltfLoader;

  public:
    void update(float delta);
    void draw(const glm::mat4& transformation, ShaderProgram* shaderProgram,
              std::array<Light*, 8>& lights, const TexturePack& environment);
    void drawAabb(const glm::mat4& transformation, ShaderProgram* shaderProgram);

    Aabb aabb() const;

    Node* findNode(const std::string& node);

    Buffer* getBuffer(int idx) { return m_buffers.at(idx).get(); }
    Sampler* getSampler(int idx) { return m_samplers.at(idx).get(); }
    Texture* getTexture(int idx) { return m_textures.at(idx).get(); }
    Mesh* getMesh(int idx) { return m_meshes.at(idx).get(); }

    Material* getMaterial(int idx)
    {
        if (idx >= 0 || idx < m_materials.size()) return &m_materials[idx];
        return nullptr;
    }

    Camera* getCamera(int idx)
    {
        if (idx >= 0 || idx < m_cameras.size()) return &m_cameras[idx];
        return nullptr;
    }

    Light* getLight(int idx) { return nullptr; }

    int addNode(Node node, Node* parent)
    {
        if (parent->getModel() != this) throw std::invalid_argument{"parent not part of model"};

        m_nodes.push_back(node);
        int idx = m_nodes.size() - 1;

        parent->addChild(idx);

        return idx;
    }

    Node* getNode(int idx)
    {
        if (idx >= 0 || idx < m_nodes.size()) return &m_nodes[idx];
        return nullptr;
    }

    const Node* getNode(int idx) const
    {
        if (idx >= 0 || idx < m_nodes.size()) return &m_nodes[idx];
        return nullptr;
    }

    std::string name;

  private:
    std::vector<std::shared_ptr<Buffer>> m_buffers;
    std::vector<std::shared_ptr<Sampler>> m_samplers;
    std::vector<std::shared_ptr<Texture>> m_textures;
    std::vector<std::shared_ptr<Mesh>> m_meshes;

    std::vector<Accessor> m_accessors;
    std::vector<Material> m_materials;
    std::vector<Animation> m_animations;
    std::vector<Camera> m_cameras;
    std::vector<Node> m_nodes;
    std::vector<std::vector<unsigned>> m_scenes;
};

} // namespace gfx

#endif /* GFX_MODEL_H */
