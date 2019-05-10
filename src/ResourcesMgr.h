#ifndef RESOURCESMGR_H
#define RESOURCESMGR_H

#include "loaders/FontLoader.h"
#include "loaders/MeshData.h"
#include "loaders/TextureData.h"
#include "loaders/MaterialData.h"
#include "Heightfield.h"
#include "Script.h"

#include "gfx/Font.h"
#include "gfx/Material.h"
#include "gfx/Mesh.h"
#include "gfx/ShaderProgram.h"
#include "gfx/Texture.h"

#include <map>
#include <string>

struct ShaderProgramData
{
    std::string vertexSrc;
    std::string geometrySrc;
    std::string fragmentSrc;
    std::string name;
};

class ResourcesMgr
{
  public:
    ResourcesMgr(const std::string& dataFolder, const std::string& shadersFolder);

    void load(const std::string& xmlFile);
    void loadShaders(const std::string& xmlFile);
    void loadMaterials(const std::string& xmlFile);

    void addShaderProgram(const ShaderProgramData& spData);
    std::shared_ptr<gfx::ShaderProgram> getShaderProgram(const std::string& name) const;

    void addTexture(const TextureData& texData);
    std::shared_ptr<gfx::Texture> getTexture(const std::string& name) const;

    void addMaterial(const MaterialData& materialData);
    std::shared_ptr<gfx::Material> getMaterial(const std::string& name) const;

    void addMesh(const MeshData& meshData);
    std::shared_ptr<gfx::Mesh> getMesh(const std::string& name) const;

    void addFont(const std::string& name, const std::string& filename);
    std::shared_ptr<gfx::Font> getFont(const std::string& name) const;

    void addScript(const std::string& name, std::shared_ptr<Script> script);
    std::shared_ptr<Script> getScript(const std::string& name) const;

    void addHeightfield(const std::string& name, const std::string& filename, float amplitude);
    std::shared_ptr<const Heightfield> getHeightfield(const std::string& name) const;

  private:
    const std::string m_dataFolder, m_shadersFolder;

    std::map<std::string, std::shared_ptr<gfx::Mesh>> m_meshes;
    std::map<std::string, std::shared_ptr<gfx::Texture>> m_textures;
    std::map<std::string, std::shared_ptr<gfx::Font>> m_fonts;
    std::map<std::string, std::shared_ptr<gfx::ShaderProgram>> m_shaderPrograms;
    std::map<std::string, std::shared_ptr<gfx::Material>> m_materials;
    std::map<std::string, std::shared_ptr<Script>> m_scripts;
    std::map<std::string, std::shared_ptr<Heightfield>> m_heightfields;
};

#endif
