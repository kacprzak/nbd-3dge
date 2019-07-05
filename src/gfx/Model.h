#ifndef GFX_MODEL_H
#define GFX_MODEL_H

#include "Animation.h"
#include "Buffer.h"
#include "Camera.h"
#include "Mesh.h"
#include "Node.h"
#include "Skin.h"
#include "Texture.h"

namespace loaders {
class GltfLoader;
} // namespace loaders

namespace gfx {

class Model final
{
    friend class loaders::GltfLoader;

  public:
    void update(float delta);
    void draw(const glm::mat4& transformation, ShaderProgram* shaderProgram,
              std::array<Light*, 8>& lights, const TexturePack& environment);
    void drawAabb(const glm::mat4& transformation, ShaderProgram* shaderProgram);

    Aabb aabb(const glm::mat4& transformation) const;

    Buffer* getBuffer(int idx) { return m_buffers.at(idx).get(); }
    Sampler* getSampler(int idx) { return m_samplers.at(idx).get(); }
    Texture* getTexture(int idx) { return m_textures.at(idx).get(); }
    Mesh* getMesh(int idx) { return m_meshes.at(idx).get(); }

    Skin* getSkin(int idx) { return &m_skins.at(idx); }

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

    Node* findNode(const std::string& node);
    int addNode(Node node, Node* parent);
    Node* getNode(int idx);
    const Node* getNode(int idx) const;

    std::string name;

  private:
    std::vector<std::shared_ptr<Buffer>> m_buffers;
    std::vector<std::shared_ptr<Sampler>> m_samplers;
    std::vector<std::shared_ptr<Texture>> m_textures;
    std::vector<std::shared_ptr<Mesh>> m_meshes;

    std::vector<Accessor> m_accessors;
    std::vector<Material> m_materials;
    std::vector<Animation> m_animations;
    std::vector<Skin> m_skins;
    std::vector<Camera> m_cameras;
    std::vector<Node> m_nodes;
    std::vector<std::vector<unsigned>> m_scenes;
};

} // namespace gfx

#endif /* GFX_MODEL_H */
