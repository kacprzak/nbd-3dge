#ifndef RESOURCESMGR_H
#define RESOURCESMGR_H

#include "Font.h"
#include "FontLoader.h"
#include "Heightfield.h"
#include "Material.h"
#include "Mesh.h"
#include "Script.h"
#include "ShaderProgram.h"
#include "Texture.h"

#include <map>
#include <string>

class ResourcesMgr
{
  public:
    ResourcesMgr(const std::string& dataFolder, const std::string& shadersFolder);

    void load(const std::string& xmlFile);
    void loadShaders(const std::string& xmlFile);
    void loadMaterials(const std::string& xmlFile);

    void addShaderProgram(const std::string& name, const std::string& vertexShaderFile,
                          const std::string& geometryShaderFile,
                          const std::string& fragmentShaderFile);
    std::shared_ptr<ShaderProgram> getShaderProgram(const std::string& name) const;

    void addTexture(const std::string& name, const std::string& filename, const std::string& wrap,
                    const std::string& internalFormat);
    void addTexture(const std::string& name, std::array<std::string, 6> filenames,
                    const std::string& wrap, const std::string& internalFormat);
    std::shared_ptr<Texture> getTexture(const std::string& name) const;

    void addMaterial(const MaterialData& materialData);
    std::shared_ptr<Material> getMaterial(const std::string& name) const;

    void addMesh(const std::string& name, const std::string& filename);
    std::shared_ptr<Mesh> getMesh(const std::string& name) const;

    void addFont(const std::string& name, const std::string& filename);
    std::shared_ptr<Font> getFont(const std::string& name) const;

    void addScript(const std::string& name, std::shared_ptr<Script> script);
    std::shared_ptr<Script> getScript(const std::string& name) const;

    void addHeightfield(const std::string& name, const std::string& filename, float amplitude);
    std::shared_ptr<const Heightfield> getHeightfield(const std::string& name) const;

  private:
    const std::string m_dataFolder, m_shadersFolder;

    std::map<std::string, std::shared_ptr<Mesh>> m_meshes;
    std::map<std::string, std::shared_ptr<Texture>> m_textures;
    std::map<std::string, std::shared_ptr<Font>> m_fonts;
    std::map<std::string, std::shared_ptr<ShaderProgram>> m_shaderPrograms;
    std::map<std::string, std::shared_ptr<Material>> m_materials;
    std::map<std::string, std::shared_ptr<Script>> m_scripts;
    std::map<std::string, std::shared_ptr<Heightfield>> m_heightfields;
};

#endif
