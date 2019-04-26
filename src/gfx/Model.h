#ifndef SCENE_H
#define SCENE_H

#include "Buffer.h"
#include "Camera.h"
#include "Mesh.h"
#include "Node.h"
#include "Texture.h"

#include <filesystem>

namespace gfx {

class Model
{
  public:
    void load(const std::filesystem::path& file);

    void update(float delta);
    void draw(ShaderProgram* shaderProgram, const Camera* camera, std::array<Light*, 8>& lights,
              const TexturePack& environment);
    void drawAabb(ShaderProgram* shaderProgram, const Camera* camera);

    Camera* currentCamera()
    {
        if (m_cameras.empty())
            return nullptr;
        else
            return &m_cameras.at(0);
    }

    Aabb aabb() const;

    gfx::Node* findNode(const std::string& node);

  private:
    std::vector<std::shared_ptr<Buffer>> m_buffers;
    std::vector<std::shared_ptr<Sampler>> m_samplers;
    std::vector<std::shared_ptr<Texture>> m_textures;
    std::vector<std::shared_ptr<Mesh>> m_meshes;

    std::vector<Accessor> m_accessors;
    std::vector<Material> m_materials;
    std::vector<Camera> m_cameras;
    std::vector<gfx::Node> m_nodes;
    std::vector<gfx::Node*> m_scene;
};

} // namespace gfx

#endif /* SCENE_H */
