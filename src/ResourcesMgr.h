#ifndef RESOURCESMGR_H
#define RESOURCESMGR_H

#include "ShaderProgram.h"
#include "Texture.h"
#include "Mesh.h"
#include "Script.h"
#include "Font.h"
#include "FontLoader.h"

#include <string>
#include <map>

class ResourcesMgr
{
 public:
    ResourcesMgr(const std::string& dataFolder, const std::string& shadersFolder);

    void load(std::string xmlFile);

    void addShaderProgram(const std::string& name,
                          const std::string& vertexShaderFile,
                          const std::string& geometryShaderFile,
                          const std::string& fragmentShaderFile);
    
    std::shared_ptr<ShaderProgram> getShaderProgram(const std::string& name);
    
    void addTexture(const std::string& name,
                    const std::string& filename,
                    const std::string& wrap);
    
    void addTexture(const std::string& name,
                    std::array<std::string, 6> filenames,
                    const std::string& wrap);
    
    std::shared_ptr<Texture> getTexture(const std::string& name);

    void addMesh(const std::string& name,
                 const std::string& filename);
    
    std::shared_ptr<Mesh> getMesh(const std::string& name);
    
    void addFont(const std::string& name,
                 const std::string& filename);

    std::shared_ptr<Font> getFont(const std::string& name);
    
    void addScript(const std::string& name,
                   std::shared_ptr<Script> script);
    
    std::shared_ptr<Script> getScript(const std::string& name);
    
 private:
    const std::string m_dataFolder, m_shadersFolder;

    std::map<std::string, std::shared_ptr<Mesh>> m_meshes;
    std::map<std::string, std::shared_ptr<Texture>> m_textures;
    std::map<std::string, std::shared_ptr<Font>> m_fonts;
    std::map<std::string, std::shared_ptr<ShaderProgram>> m_shaderPrograms;
    std::map<std::string, std::shared_ptr<Script>> m_scripts;
};

#endif
