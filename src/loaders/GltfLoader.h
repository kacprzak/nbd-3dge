#ifndef LOADERS_GLTFLOADER_H
#define LOADERS_GLTFLOADER_H

#include <filesystem>
#include <memory>
#include <vector>

#include "../gfx/Model.h"

namespace fx {
namespace gltf {
struct Document;
} // namespace gltf
} // namespace fx

namespace loaders {

class GltfLoader
{
  public:
    void load(const std::filesystem::path& file);
    std::shared_ptr<gfx::Model> model() const;

  private:
    void loadBuffers(const fx::gltf::Document& doc);
    void loadAccessors(const fx::gltf::Document& doc);
    void loadSamplers(const fx::gltf::Document& doc);
    void loadTextures(const fx::gltf::Document& doc, const std::filesystem::path& file);
    void loadMaterials(const fx::gltf::Document& doc);
    void loadMeshes(const fx::gltf::Document& doc);
    void loadCameras(const fx::gltf::Document& doc);
    void loadNodes(const fx::gltf::Document& doc);

    std::vector<std::shared_ptr<gfx::Buffer>> m_buffers;
    std::vector<std::shared_ptr<gfx::Sampler>> m_samplers;
    std::vector<std::shared_ptr<gfx::Texture>> m_textures;
    std::vector<std::shared_ptr<gfx::Mesh>> m_meshes;

    std::vector<gfx::Accessor> m_accessors;
    std::vector<gfx::Material> m_materials;
    std::vector<gfx::Camera> m_cameras;
    std::vector<gfx::Node> m_nodes;
    std::vector<std::vector<unsigned>> m_scenes;

	std::string m_name;
};

} // namespace loaders

#endif // LOADERS_GLTFLOADER_H
